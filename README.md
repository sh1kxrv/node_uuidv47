# node-uuidv47

NAPI-based Node.js bindings for [uuidv47](https://github.com/stateless-me/uuidv47)

## Performance

```bash
node -e "const uuidv47 = require('./index.js'); con
sole.log('Performance test - generating 10,000 UUIDs:'); const start = process.hrtime.bigint(); for(let
 i = 0; i < 10000; i++) { uuidv47.uuidv47(); } const end = process.hrtime.bigint(); const duration = Nu
mber(end - start) / 1000000; console.log('Time taken:', duration.toFixed(2), 'ms'); console.log('Rate:'
, (10000 / (duration / 1000)).toFixed(0), 'UUIDs/second');"
```

> Performance test - generating 10,000 UUIDs:
Time taken: 9.33 ms
Rate: 1071266 UUIDs/second

## Requirements

- node-gyp
- node-addon-api
- bindings