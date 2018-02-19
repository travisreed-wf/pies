package main

import (
    "time"

	log "github.com/Sirupsen/logrus"

    "github.com/Workiva/app_intelligence_go"
)

func main() {
    config := appintel.GetDefaultConfig()
	// config := appintel.GetLocalDevConfig()
	config.Interval = 5 * time.Second
    reporter, err := appintel.NewTelemetryReporterWithConfig(config)
    // current reporter constructors will always return a nil error.
    // The error is returned to allow for future changes without
    // breaking stuff.
    if err != nil {
        panic("Won't happen")
    }
    go reporter.Start()

    log.Info("Starting REST server")

	s := GetRestServer()

	log.Fatal(s.Run())
}
