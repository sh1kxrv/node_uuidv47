const addon = require("bindings")("uuidv47");

module.exports = {
	uuidParse: addon.uuidParse,
	encodeFacade: addon.encodeFacade,
	decodeFacade: addon.decodeFacade,
};
