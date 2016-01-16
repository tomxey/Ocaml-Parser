#include "parseessentials.h"
#include "vartype.h"


std::vector<Statement*> ParseEssentials::toplevel_statements;
Environment ParseEssentials::toplevel_environment;


void ParseEssentials::parseNewStatements()
{
    static int parsedStatements = 0;

    while(parsedStatements != ParseEssentials::toplevel_statements.size()){
        Statement * statement = ParseEssentials::toplevel_statements[parsedStatements++];
        parseStatement(statement);
    }
} // parse new statements


void ParseEssentials::parseStatement(Statement *statement)
{
    ParseEssentials::toplevel_environment.printNewValues(); // for the first time, print system functions

    std::cout << getColorCode(GREEN_CODE, true, true) << "---------Parsing Statement-------------" << getColorCode(RESET_CODE) << std::endl;
    int statement_good = true;
    Environment environment_backup = ParseEssentials::toplevel_environment;
    std::cout << statement->print(0) << std::endl;

    std::cout << getColorCode(WHITE_CODE, true, true) << "---------Determine Types---------------" << getColorCode(RESET_CODE) << std::endl;
    Type type;
    try{
        type = statement->deduceType(ParseEssentials::toplevel_environment, ParseEssentials::toplevel_environment.getNewPolymorphicType());
        //std::cout << "Relations:\n" << ParseEssentials::toplevel_environment.relationsToString();
        std::cout << "Deduced type: " << ParseEssentials::toplevel_environment.renumeratedToSmallest(type) << std::endl;
    } catch(std::runtime_error ex){
        std::cout << getColorCode(RED_CODE, true, true) << "Type deduction for statement failed:\n" << ex.what() << std::endl;
        statement_good = false;
        ParseEssentials::toplevel_environment = environment_backup;
    }

    std::cout << getColorCode(WHITE_CODE, true, true) << "---------    Execute    ---------------" << getColorCode(RESET_CODE) << std::endl;
    if(statement_good){
        try{
            Value* retVal = statement->execute(ParseEssentials::toplevel_environment);
            if(retVal != nullptr){
                //std::cout << retVal->printValue() << std::endl;
                ParseEssentials::toplevel_environment.addValue(Identifier("-"), retVal);
                ParseEssentials::toplevel_environment.resetIdentifierType(Identifier("-"), type);
            }
            else{
                std::cout << "Some statement, no value to print..." << std::endl;
            }
            ParseEssentials::toplevel_environment.printNewValues();
        }
        catch(std::runtime_error ex){
            std::cout << getColorCode(RED_CODE, true, true) << "Error during execution: " << ex.what() << std::endl;
            ParseEssentials::toplevel_environment = environment_backup;
        }
    }
    else{
        std::cout << getColorCode(YELLOW_CODE, true, true) << "Statement not proper. Skipping..." << std::endl;
    }

    //ParseEssentials::toplevel_environment.cleanupAfterStatement();
    std::cout << getColorCode(WHITE_CODE, true, true) << "--------Statement Parsing Done---------" << getColorCode(RESET_CODE) << std::endl;
} // parseStatement


std::string ParseEssentials::getColorCode(int color_id, bool bright, bool bold, bool background)
{
#ifdef COLOR_OUTPUT
    return std::string("\x1b[") + std::to_string(color_id+(background?(bright?60:0)+10:color_id==0?0:(bright?60:0))) + (bold?";1":"") + "m";
#else
    return std::string();
#endif
}
