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
    int statement_good = true;
    std::cout << statement->print(0) << std::endl;

    std::cout << "---------Determine Types---------------" << std::endl;
    int deductionAttempts = 0;
    try{
        deductionAttempts++;
        Type type, newType;
        type = newType = statement->deduceType(ParseEssentials::toplevel_environment, ParseEssentials::toplevel_environment.getNewPolymorphicType());
        //do{
           // type = newType;
           // deductionAttempts++;
           // newType = statement->deduceType(ParseEssentials::toplevel_environment, type);
        //} while(type != newType);
      //std::cout << "Relations:\n" << ParseEssentials::toplevel_environment.relationsToString();
      std::cout << "Deduced type: " << ParseEssentials::toplevel_environment.renumeratedToSmallest(newType) << std::endl;
    } catch(std::runtime_error ex){
        std::cout << "Type deduction for statement failed:\n" << ex.what() << std::endl;
        statement_good = false;
    }
    std::cout << "Deduced " << deductionAttempts << " times..." << std::endl;

    std::cout << "---------    Execute    ---------------" << std::endl;
    if(statement_good){
        Value* retVal = statement->execute(ParseEssentials::toplevel_environment);
        if(retVal != nullptr){
            std::cout << retVal->print(1) << std::endl;
        }
        else{
            std::cout << "Some statement, no value to print..." << std::endl;
        }
    }
    else{
        std::cout << "Statement not proper. Skipping..." << std::endl;
    }
}
