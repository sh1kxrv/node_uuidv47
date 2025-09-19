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
 * Encodes a UUID with additional facade parameters
 * @param uuid - The UUID string to encode
 * @param param1 - First bigint parameter for encoding
 * @param param2 - Second bigint parameter for encoding
 * @returns Encoded result
 */
export function encodeFacade(
	uuid: string,
	param1: bigint,
	param2: bigint,
): string;

/**
 * Decodes a previously encoded facade back to original form
 * @param encoded - The encoded data to decode
 * @param param1 - First bigint parameter used in encoding
 * @param param2 - Second bigint parameter used in encoding
 * @returns Decoded UUID string or result
 */
export function decodeFacade(
	encoded: string,
	param1: bigint,
	param2: bigint,
): string;

/**
 * Default export containing all functions
 */
declare const uuidv47: {
	uuidParse: typeof uuidParse;
	encodeFacade: typeof encodeFacade;
	decodeFacade: typeof decodeFacade;
};

export default uuidv47;
