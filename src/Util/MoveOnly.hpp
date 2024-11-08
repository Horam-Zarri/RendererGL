#ifndef MOVE_ONLY_H
#define MOVE_ONLY_H

#define MAKE_MOVE_ONLY(clazz) public: \
                                clazz(const clazz&) = delete; \
                                clazz& operator=(const clazz&) = delete; \
                                clazz(clazz&&) noexcept = default; \
                                clazz& operator=(clazz&&) noexcept = default;

#endif
