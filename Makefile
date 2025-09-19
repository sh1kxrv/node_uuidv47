# C header URL and commit
C_HEADER_URL := https://raw.githubusercontent.com/stateless-me/uuidv47/main/uuidv47.h
C_HEADER_PATH := src/uuidv47.h

.PHONY: download-c-header
download-c-header:
	@if [ ! -f $(C_HEADER_PATH) ]; then \
		echo "Downloading uuidv47.h from upstream..."; \
		curl -s $(C_HEADER_URL) -o $(C_HEADER_PATH); \
		echo "Downloaded $(C_HEADER_PATH)"; \
	else \
		echo "$(C_HEADER_PATH) already exists"; \
	fi

.PHONY: verify-c-compat
verify-c-compat: download-c-header
	@echo "Compiling C test vector generator..."
	@gcc -o src/test_vectors_gen src/test_vectors_gen.c
	@echo "Running C test vectors..."
	@./src/test_vectors_gen
	@echo ""