const addon = require("bindings")("uuidv47");

module.exports = {
	uuidParse: addon.uuidParse,
	setKeys: addon.setKeys,
	encode: addon.encode,
	decode: addon.decode,
	hasKeys: addon.hasKeys,
};
