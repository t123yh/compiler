//
// Created by t123yh on 2020/10/30.
//

#include "defs.h"

token_type_t get_expression_type(const expression* expr) {
    auto* t = dynamic_cast<const constant_expression*>(expr);
    auto cond_type = INTTK;
    if (t && t->type == CHARCON)
        cond_type = CHARTK;
    return cond_type;
}

