#include "ast.h"


Value *Conditional::execute(Environment &env)
{
    Value* return_val;
    if( ((Bool*)condition->execute(env))->value == true ){
        return_val = true_path->execute(env);
    }
    else{
        return_val = false_path->execute(env);
    }
    return return_val;
}
