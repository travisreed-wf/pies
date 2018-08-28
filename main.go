package main

import (
	"fmt"
	"flag"
	"io/ioutil"
	"math/rand"
    "time"

	log "github.com/Sirupsen/logrus"

    "github.com/Workiva/app_intelligence_go/telemetry"
    "github.com/Workiva/app_intelligence_go"
    "github.com/rcrowley/go-metrics"
)

var logRate float64
var telemetryInterval time.Duration
var seed int64
var randGen *rand.Rand

func parseArgs() {
	var t float64
	flag.Float64Var(&logRate, "log-rate", 1.0, "Arrival rate for logs per second")
	flag.Float64Var(&t, "telemetry-interval", 5.0, "Interval between telemetry emits")
	flag.Int64Var(&seed, "seed", time.Now().UnixNano(), "Random number seed (defaults to current nanoseconds)")

	flag.Parse()

	telemetryInterval = time.Duration(int64(t * float64(time.Second)))
}

func calcSleepTime(arrivalRate float64) time.Duration {
   var sleepval float64
   sleepval = randGen.ExpFloat64() / arrivalRate
   // fmt.Printf("calc sleepval = %f\n", sleepval)
   duration := time.Duration(int64(sleepval * float64(time.Second)))
   // fmt.Printf("duration_in = %T %v\n", duration, duration)
   return duration
}

func main() {
	parseArgs()
	// fmt.Printf("Log Rate = %f\n", logRate)
	// fmt.Printf("Telemetry Interval = %f\n", telemetryInterval)
	// fmt.Printf("Seed = %d\n", seed)

	randGen = rand.New(rand.NewSource(seed)) 

	var sleepTime time.Duration
	// var totalTime time.Duration
	// totalTime = 0.0
	// for i := 0; i < 20; i++ {
	// 	sleepTime = calcSleepTime(logRate)
	// 	fmt.Printf("sleepTime = %v\n", sleepTime)
	// 	totalTime += sleepTime
	// }
	// fmt.Printf("totalTime = %v, average = %v\n", totalTime, totalTime / 20.0)

    meter := metrics.NewMeter()
    metrics.Register("quux", meter)
    meter.Mark(47)

    config := telemetry.GetDefaultConfig()
	config.Interval = telemetryInterval
    reporter, err := telemetry.NewReporterWithConfig(config)
    // current reporter constructors will always return a nil error.
    // The error is returned to allow for future changes without
    // breaking stuff.
    if err != nil {
        panic("Won't happen")
    }
    go reporter.Start()

    hook := appintel.NewHook()

    log.AddHook(hook)
    log.SetOutput(ioutil.Discard) // prevent duplicate logs

	for true {
		log.WithFields(log.Fields{
			"correlationId": "abc123",
			"logRate": fmt.Sprintf("%f", logRate),
			"foo": "bar",
		}).Info("I am a happy walrus")
		sleepTime = calcSleepTime(logRate)
		time.Sleep(sleepTime)
	}

}
