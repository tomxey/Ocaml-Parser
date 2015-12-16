#include "ast.h"


Value *Conditional::execute(Environment &env)
{
    if( ((Bool*)condition->execute(env))->value == true ){
        return true_path->execute(env);
    }
    else{
        return false_path->execute(env);
    }
}
