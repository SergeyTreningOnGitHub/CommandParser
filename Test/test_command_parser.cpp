#include <sstream>
#include <gtest/gtest.h>
#include "command_parser.h"

using namespace std;

/*!
    This is test for command parser
*/

TEST(CommandParser, TestEmptyInput){
    istringstream in("EOF");
    ostringstream out;    

    CommandParser cmd_parser(1, in, out);
    cmd_parser.HandleCommand();
    ASSERT_TRUE(cmd_parser.IsEndStream());

    ASSERT_EQ(out.str(), string(""));
}

TEST(CommandParser, TestIncompleteStaticPack){
    istringstream in("cmd1\ncmd2\nEOF");
    ostringstream out;    

    CommandParser cmd_parser(3, in, out);
    while(!cmd_parser.IsEndStream()){
        cmd_parser.HandleCommand();    
    }

    ASSERT_EQ(out.str(), string("bulk: cmd1, cmd2\n"));
}

TEST(CommandParser, TestCompleteStaticPack){
    istringstream in ("cmd1\ncmd2\ncmd3\nEOF");
    ostringstream out;    

    CommandParser cmd_parser(3, in, out);
    while(!cmd_parser.IsEndStream()){
        cmd_parser.HandleCommand();    
    }

    ASSERT_EQ(out.str(), string("bulk: cmd1, cmd2, cmd3\n"));
}

TEST(CommandParser, TestCompleteIncompleteStaticPack){
    istringstream in("cmd1\ncmd2\ncmd3\ncmd4\nEOF");
    ostringstream out;    

    CommandParser cmd_parser(3, in, out);
    while(!cmd_parser.IsEndStream()){
        cmd_parser.HandleCommand();    
    }

    ASSERT_EQ(out.str(), string("bulk: cmd1, cmd2, cmd3\nbulk: cmd4\n"));
}

TEST(CommandParser, TestIncompleteDynamicPack){
    istringstream in("{\ncmd1\ncmd2\nEOF");
    ostringstream out;    

    CommandParser cmd_parser(3, in, out);
    while(!cmd_parser.IsEndStream()){
        cmd_parser.HandleCommand();    
    }

    ASSERT_EQ(out.str(), string(""));
}

TEST(CommandParser, TestEnclosedIncompleteDynamicPack){
    istringstream in("{\ncmd1\n{\ncmd2\n}\nEOF");
    ostringstream out;    

    CommandParser cmd_parser(3, in, out);
    while(!cmd_parser.IsEndStream()){
        cmd_parser.HandleCommand();    
    }

    ASSERT_EQ(out.str(), string(""));
}

TEST(CommandParser, TestCompleteDynamicPack){
    istringstream in("{\ncmd1\ncmd2\n}\nEOF");
    ostringstream out;    

    CommandParser cmd_parser(3, in, out);
    while(!cmd_parser.IsEndStream()){
        cmd_parser.HandleCommand();    
    }

    ASSERT_EQ(out.str(), string("bulk: cmd1, cmd2\n"));
}

TEST(CommandParser, TestEnclosedCompleteDynamicPack){
    istringstream in("{\ncmd1\n{\ncmd2\n}\ncmd3\n}\nEOF");
    ostringstream out;    

    CommandParser cmd_parser(3, in, out);
    while(!cmd_parser.IsEndStream()){
        cmd_parser.HandleCommand();    
    }

    ASSERT_EQ(out.str(), string("bulk: cmd1, cmd2, cmd3\n"));
}

TEST(CommandParser, TestIncompleteStaticDynamicPack){
    istringstream in("cmd1\ncmd2\n{\ncmd1\n{\ncmd2\n}\ncmd3\nEOF");
    ostringstream out;    

    CommandParser cmd_parser(3, in, out);
    while(!cmd_parser.IsEndStream()){
        cmd_parser.HandleCommand();    
    }

    ASSERT_EQ(out.str(), string("bulk: cmd1, cmd2\n"));
}

TEST(CommandParser, TestIncompleteStaticCompleteDynamicPack){
    istringstream in("cmd1\ncmd2\n{\ncmd1\n{\ncmd2\n}\ncmd3\n}\nEOF");
    ostringstream out;    

    CommandParser cmd_parser(3, in, out);
    while(!cmd_parser.IsEndStream()){
        cmd_parser.HandleCommand();    
    }

    ASSERT_EQ(out.str(), string("bulk: cmd1, cmd2\nbulk: cmd1, cmd2, cmd3\n"));
}

TEST(CommandParser, TestCompleteStaticIncompleteDynamicPack){
    istringstream in("cmd1\ncmd2\ncmd3\n{\ncmd1\n{\ncmd2\n}\ncmd3\nEOF");
    ostringstream out;    

    CommandParser cmd_parser(3, in, out);
    while(!cmd_parser.IsEndStream()){
        cmd_parser.HandleCommand();    
    }

    ASSERT_EQ(out.str(), string("bulk: cmd1, cmd2, cmd3\n"));
}

TEST(CommandParser, TestCompleteStaticDynamicPack){
    istringstream in("cmd1\ncmd2\ncmd3\n{\ncmd1\n{\ncmd2\n}\ncmd3\n}\nEOF");
    ostringstream out;    

    CommandParser cmd_parser(3, in, out);
    while(!cmd_parser.IsEndStream()){
        cmd_parser.HandleCommand();    
    }

    ASSERT_EQ(out.str(), string("bulk: cmd1, cmd2, cmd3\nbulk: cmd1, cmd2, cmd3\n"));
}