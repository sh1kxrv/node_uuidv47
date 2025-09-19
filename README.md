# node-uuidv47

A high-performance Node.js N-API addon for UUIDv47 operations - encoding UUIDv7 into UUIDv4 facades and decoding them back.

## Features

- **🚀 High Performance**: Native C++ implementation using N-API
- **🔑 Global Key Management**: Set encryption keys once, use everywhere
- **🛡️ Type Safety**: Full TypeScript support with comprehensive type definitions
- **✅ Well Tested**: Comprehensive test suite with automated CI/CD
- **📦 Zero Dependencies**: No runtime dependencies (except for development)

## Installation

```bash
npm install node-uuidv47
# or
pnpm install node-uuidv47
# or
yarn add node-uuidv47
```

## Quick Start

```javascript
const uuidv47 = require('node-uuidv47');
const { v7: uuidv7 } = require('uuid');

// Generate a UUIDv7
const originalUUID = uuidv7();
console.log('Original UUID:', originalUUID);

// Set encryption keys once
uuidv47.setKeys(1234567890123456789n, 9876543210987654321n);

// Encode to facade
const facade = uuidv47.encode(originalUUID);
console.log('Encoded facade:', facade);

// Decode back to original
const decoded = uuidv47.decode(facade);
console.log('Decoded UUID:', decoded);
console.log('Match:', originalUUID === decoded); // true
```

## API Reference

### `setKeys(key0: bigint, key1: bigint): boolean`

Sets the global encryption keys used for encoding and decoding operations.

**Parameters:**
- `key0` - First 64-bit encryption key (as BigInt)
- `key1` - Second 64-bit encryption key (as BigInt)

**Returns:** `true` if keys were set successfully

**Example:**
```javascript
const success = uuidv47.setKeys(123456789n, 987654321n);
console.log('Keys set:', success); // true
```

### `encode(uuid: string): string`

Encodes a UUIDv7 into a UUIDv4 facade using the previously set global keys.

**Parameters:**
- `uuid` - A valid UUIDv7 string to encode

**Returns:** Encoded UUIDv4 facade string

**Throws:** Error if keys are not set or UUID is invalid

**Example:**
```javascript
uuidv47.setKeys(123n, 456n);
const facade = uuidv47.encode('01234567-89ab-7def-8123-456789abcdef');
```

### `decode(facade: string): string`

Decodes a UUIDv4 facade back to the original UUIDv7 using the previously set global keys.

**Parameters:**
- `facade` - A valid UUID facade string to decode

**Returns:** Original UUIDv7 string

**Throws:** Error if keys are not set or facade is invalid

**Example:**
```javascript
const original = uuidv47.decode('01234567-89ab-4def-8123-456789abcdef');
```

### `hasKeys(): boolean`

Checks if global encryption keys have been set.

**Returns:** `true` if keys are set, `false` otherwise

**Example:**
```javascript
console.log('Keys set:', uuidv47.hasKeys()); // false
uuidv47.setKeys(123n, 456n);
console.log('Keys set:', uuidv47.hasKeys()); // true
```

### `uuidParse(uuid: string): boolean`

Validates if a string is a properly formatted UUID.

**Parameters:**
- `uuid` - String to validate

**Returns:** `true` if valid UUID format, `false` otherwise

**Example:**
```javascript
console.log(uuidv47.uuidParse('01234567-89ab-cdef-8123-456789abcdef')); // true
console.log(uuidv47.uuidParse('invalid-uuid')); // false
```

## Usage Examples

### Basic Usage

```javascript
const uuidv47 = require('node-uuidv47');
const { v7: uuidv7 } = require('uuid');

// Set up encryption keys
uuidv47.setKeys(
  1234567890123456789n,
  9876543210987654321n
);

// Generate and encode multiple UUIDs
const uuids = [uuidv7(), uuidv7(), uuidv7()];
const facades = uuids.map(uuid => uuidv47.encode(uuid));
const decoded = facades.map(facade => uuidv47.decode(facade));

console.log('All match:', uuids.every((uuid, i) => uuid === decoded[i])); // true
```

### Error Handling

```javascript
const uuidv47 = require('node-uuidv47');

try {
  // This will throw an error - keys not set
  uuidv47.encode('01234567-89ab-7def-8123-456789abcdef');
} catch (error) {
  console.error('Error:', error.message); // "Keys not set. Call setKeys() first."
}

// Set keys first
uuidv47.setKeys(123n, 456n);

try {
  // This will throw an error - invalid UUID
  uuidv47.encode('invalid-uuid');
} catch (error) {
  console.error('Error:', error.message); // "Invalid UUIDv7"
}
```

### TypeScript Usage

```typescript
import uuidv47 from 'node-uuidv47';
import { v7 as uuidv7 } from 'uuid';

// Type-safe usage
const keys: [bigint, bigint] = [123456789n, 987654321n];
uuidv47.setKeys(...keys);

const uuid: string = uuidv7();
const facade: string = uuidv47.encode(uuid);
const decoded: string = uuidv47.decode(facade);

const isValid: boolean = uuidv47.uuidParse(uuid);
const keysSet: boolean = uuidv47.hasKeys();
```

### Batch Processing

```javascript
const uuidv47 = require('node-uuidv47');
const { v7: uuidv7 } = require('uuid');

// Set keys once for the entire session
uuidv47.setKeys(
  BigInt('0x123456789ABCDEF0'),
  BigInt('0xFEDCBA9876543210')
);

// Process large batches efficiently
function processBatch(uuids) {
  return uuids.map(uuid => {
    const facade = uuidv47.encode(uuid);
    // ... store facade in database or send over network
    return facade;
  });
}

function decodeBatch(facades) {
  return facades.map(facade => uuidv47.decode(facade));
}

// Example usage
const originalUUIDs = Array.from({ length: 1000 }, () => uuidv7());
const facades = processBatch(originalUUIDs);
const decodedUUIDs = decodeBatch(facades);

console.log('All match:', originalUUIDs.every((uuid, i) => uuid === decodedUUIDs[i]));
```

## Performance

The library is optimized for high-performance operations with global key management:

### Performance Benefits

1. **Global Keys**: Set encryption keys once, eliminating parameter passing overhead
2. **Native Implementation**: C++ implementation provides optimal performance
3. **Memory Efficient**: Minimal memory allocation and copying
4. **Zero-Copy Operations**: Direct string manipulation where possible

### Performance Comparison

```javascript
const uuidv47 = require('node-uuidv47');
const { v7: uuidv7 } = require('uuid');

const testUUID = uuidv7();
const iterations = 100000;

// Set keys once
uuidv47.setKeys(123456789n, 987654321n);

console.time('Global Keys Method');
for (let i = 0; i < iterations; i++) {
  const encoded = uuidv47.encode(testUUID);
  const decoded = uuidv47.decode(encoded);
}
console.timeEnd('Global Keys Method');
// Typical output: Global Keys Method: 250-400ms
```

## Use Cases

### 1. Database Storage Optimization

```javascript
// Store UUIDs as facades to obscure actual values
const userUUID = uuidv7();
const facade = uuidv47.encode(userUUID);

// Store facade in database
await db.users.create({ id: facade, name: 'John Doe' });

// Retrieve and decode when needed
const user = await db.users.findOne({ id: facade });
const originalUUID = uuidv47.decode(user.id);
```

### 2. API Response Masking

```javascript
// Mask internal UUIDs in API responses
app.get('/api/users/:id', (req, res) => {
  const internalUUID = req.params.id;
  const user = getUserById(internalUUID);
  
  // Encode sensitive UUIDs in response
  const response = {
    id: uuidv47.encode(user.id),
    profileId: uuidv47.encode(user.profileId),
    name: user.name
  };
  
  res.json(response);
});
```

## Building from Source

```bash
# Clone the repository
git clone https://github.com/sh1kxrv/node_uuidv47.git
cd node_uuidv47

# Install dependencies
pnpm install

# Build the native addon
pnpm run build

# Run tests
pnpm test
```

## CI/CD

This project includes comprehensive CI/CD with GitHub Actions:

- **Multi-platform testing**: Ubuntu, Windows, macOS
- **Multi-version Node.js**: 16.x, 18.x, 20.x, 22.x
- **Automated testing**: Jest test suite with performance benchmarks
- **TypeScript validation**: Type checking and definition validation
- **Security auditing**: Dependency vulnerability scanning
- **Code quality**: Linting and package validation

## Requirements

- Node.js >= 16.0.0
- C++ compiler (for building from source)
- Python 3.x (for node-gyp)

## License

MIT License - see [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! Please read our contributing guidelines and submit pull requests to our repository.

## Support

- 📖 [Documentation](https://github.com/sh1kxrv/node_uuidv47#readme)
- 🐛 [Issue Tracker](https://github.com/sh1kxrv/node_uuidv47/issues)
- 💬 [Discussions](https://github.com/sh1kxrv/node_uuidv47/discussions)
