package main

import (
	"io/ioutil"
    "time"

	log "github.com/Sirupsen/logrus"

    "github.com/Workiva/app_intelligence_go/telemetry"
    "github.com/Workiva/app_intelligence_go"
    "github.com/rcrowley/go-metrics"
)

func main() {
	// Add arg parse

    meter := metrics.NewMeter()
    metrics.Register("quux", meter)
    meter.Mark(47)

    config := telemetry.GetDefaultConfig()
	config.Interval = 5 * time.Second
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

	log.WithFields(log.Fields{
		"correlationId": "abc123",
		"bar": 100,
		"foo": "yo",
	}).Info("I am a walrus")

	log.Info("Sleeping")

	time.Sleep(60 * time.Second)
}
