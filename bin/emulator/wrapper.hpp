#ifndef ZKEMV_FRAMEWORK_BIN_EMULATOR_WRAPPER_HPP_
#define ZKEMV_FRAMEWORK_BIN_EMULATOR_WRAPPER_HPP_

#ifdef __cplusplus
extern "C" {
#endif

void* make_runner();
int run(void* handler, const unsigned char* input_block_data, unsigned long input_block_size);

int create_block(const char* config_file, unsigned char* input_block_data);

#ifdef __cplusplus
}
#endif

#endif  // ZKEMV_FRAMEWORK_BIN_EMULATOR_WRAPPER_HPP_
