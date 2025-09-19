/**
 * Node.js N-API addon for UUIDv47 operations
 */

/**
 * Parses a UUID string and validates its format
 * @param uuid - The UUID string to parse
 * @returns Parsed UUID result or validation status
 */
export function uuidParse(uuid: string): boolean;

/**
 * Sets global keys for simplified encoding/decoding operations
 * @param key0 - First key parameter
 * @param key1 - Second key parameter
 * @returns True if keys were set successfully
 */
export function setKeys(key0: bigint, key1: bigint): boolean;

/**
 * Encodes a UUID using previously set global keys
 * @param uuid - The UUID string to encode
 * @returns Encoded facade string
 */
export function encode(uuid: string): string;

/**
 * Decodes a facade using previously set global keys
 * @param encoded - The encoded facade string to decode
 * @returns Decoded UUID string
 */
export function decode(encoded: string): string;

/**
 * Checks if global keys have been set
 * @returns True if keys are set, false otherwise
 */
export function hasKeys(): boolean;

/**
 * Default export containing all functions
 */
declare const uuidv47: {
	uuidParse: typeof uuidParse;
	setKeys: typeof setKeys;
	encode: typeof encode;
	decode: typeof decode;
	hasKeys: typeof hasKeys;
};

export default uuidv47;
