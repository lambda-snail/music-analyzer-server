# Running Locally

```bash
 ./todo --docroot . --config ./config/wt_config.xml --http-address 0.0.0.0 --http-port 9090
```

The `resourcesURL` property in `wt_config.xml` should point to the `resources/` directory. The path is relative to
the `docroot` directory.