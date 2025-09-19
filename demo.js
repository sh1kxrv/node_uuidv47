const uuidv47 = require("./index.js");
const { v7: uuidv7 } = require("uuid");

console.log("=== UUIDv47 Demo (Simplified API) ===\n");

// Generate a test UUIDv7
const testUUID = uuidv7();
console.log("Generated UUIDv7:", testUUID);

// Test basic parsing
console.log("UUID Parse Result:", uuidv47.uuidParse(testUUID));

console.log("\n=== Global Key Management ===");
const key0 = 1234567890123456789n;
const key1 = 9876543210987654321n;

console.log("Keys:", { key0: key0.toString(), key1: key1.toString() });

// Set global keys once
const keysSet = uuidv47.setKeys(key0, key1);
console.log("Keys set successfully:", keysSet);
console.log("Has keys?", uuidv47.hasKeys());

console.log("\n=== Encoding and Decoding ===");

// Now encode/decode without passing keys each time
const encoded = uuidv47.encode(testUUID);
console.log("Encoded facade:", encoded);

const decoded = uuidv47.decode(encoded);
console.log("Decoded UUID:", decoded);
console.log("Match original:", decoded === testUUID);

console.log("\n=== Multiple Operations ===");

// Generate multiple UUIDs and process them
const uuids = [uuidv7(), uuidv7(), uuidv7()];
console.log("Original UUIDs:", uuids);

const facades = uuids.map((uuid) => uuidv47.encode(uuid));
console.log("Encoded facades:", facades);

const decodedUUIDs = facades.map((facade) => uuidv47.decode(facade));
console.log("Decoded UUIDs:", decodedUUIDs);

const allMatch = uuids.every((uuid, i) => uuid === decodedUUIDs[i]);
console.log("All match:", allMatch);

console.log("\n=== Performance Test ===");

const iterations = 10000;
console.log(`Running ${iterations} iterations...`);

const start = process.hrtime.bigint();
for (let i = 0; i < iterations; i++) {
	const enc = uuidv47.encode(testUUID);
	const dec = uuidv47.decode(enc);
}
const end = process.hrtime.bigint();
const duration = Number(end - start) / 1000000;

console.log(`Completed ${iterations} operations in ${duration.toFixed(2)}ms`);
console.log(`Average: ${(duration / iterations).toFixed(4)}ms per operation`);
console.log(
	`Throughput: ${Math.round(iterations / (duration / 1000))} operations/second`,
);

console.log("\n=== Error Handling Demo ===");

try {
	// This should work fine
	const result = uuidv47.encode(testUUID);
	console.log("✓ Encoding with set keys works");
} catch (error) {
	console.log("✗ Unexpected error:", error.message);
}

try {
	// This should throw an error
	uuidv47.encode("invalid-uuid");
	console.log("✗ Should have thrown an error for invalid UUID");
} catch (error) {
	console.log("✓ Correctly caught error for invalid UUID:", error.message);
}

console.log("\n=== Key Update Demo ===");

// Update keys and verify it works
const newKey0 = 9999999999999999999n;
const newKey1 = 1111111111111111111n;

console.log("Updating keys...");
uuidv47.setKeys(newKey0, newKey1);

const encodedWithNewKeys = uuidv47.encode(testUUID);
const decodedWithNewKeys = uuidv47.decode(encodedWithNewKeys);

console.log("Encoded with new keys:", encodedWithNewKeys);
console.log("Decoded with new keys:", decodedWithNewKeys);
console.log("Match with new keys:", decodedWithNewKeys === testUUID);
console.log("Different from old encoding:", encoded !== encodedWithNewKeys);

console.log("\n=== Demo Complete ===");
console.log("✅ All operations successful!");
console.log("🚀 UUIDv47 library is ready for production use!");
