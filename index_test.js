const uuidv47 = require("./index.js");

console.log("Performance");
const start = process.hrtime.bigint();

console.log("Testing uuidParse function:");
console.log(
	"Valid UUID:",
	uuidv47.uuidParse("550e8400-e29b-41d4-a716-446655440000"),
);

console.log("Invalid UUID:", uuidv47.uuidParse("invalid-uuid"));

console.log(
	"Testing encodeFacade:",
	uuidv47.encodeFacade("550e8400-e29b-41d4-a716-446655440000", 123n, 456n),
);

console.log(
	"Testing decodeFacade with result:",
	uuidv47.decodeFacade(
		uuidv47.encodeFacade("550e8400-e29b-41d4-a716-446655440000", 123n, 456n),
		123n,
		456n,
	),
);

const end = process.hrtime.bigint();
const duration = Number(end - start) / 1000000;
console.log("Time taken:", duration.toFixed(2), "ms");
