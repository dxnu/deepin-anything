#ifndef ANYTHING_PRINT_HELPER_H_
#define ANYTHING_PRINT_HELPER_H_

#include <iostream>

#include "common/anything_fwd.hpp"
#include "common/file_record.hpp"

ANYTHING_NAMESPACE_BEGIN

inline void print(const file_record& record) {
    std::cout << "file_name: " << record.file_name << " full_path: " << record.full_path << "\n";
}

ANYTHING_NAMESPACE_END

#endif // ANYTHING_PRINT_HELPER_H_