const addon = require("bindings")("uuidv47");

module.exports = {
	uuidv47: addon.uuidv47,
	encodeFacade: addon.encodeFacade,
	decodeFacade: addon.decodeFacade,
};
