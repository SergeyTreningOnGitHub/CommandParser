#include "command_parser.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <cassert>

using namespace std;

void PackState::flush_commands() {
	if (cmds_.empty())
		return;

	string res;	    

	res += "bulk: ";
	for (size_t i = 0; i < cmds_.size(); i++) {
		if (i < cmds_.size() - 1) {
			res += cmds_[i] + ", ";
		}
		else {
			res += cmds_[i];			
		}
	}

	ostream& out = cmd_parser_->GetOutputStream();
	out << res << endl;

	if(cmd_parser_->IsLogEnabled()){
		ostringstream name_file;
		name_file << time_creation << ".log";
		ofstream out_file(name_file.str());
		out_file << res << endl;
	}
}

void PackState::add_cmd(const string& cmd) {
	if (cmds_.empty())
		time_creation = chrono::system_clock::to_time_t(chrono::system_clock::now());

	cmds_.push_back(cmd);
}

PackState::PackState(CommandParser* cmd_parser) : cmd_parser_(cmd_parser) {}
	

StaticState::StaticState(CommandParser* cmd_parser, uint32_t size_pack) : PackState(cmd_parser), size_pack_(size_pack) {
	cmds_.reserve(size_pack);
}

void StaticState::HandleCommand(const string& cmd){
	if (cmd != "{") {
		add_cmd(cmd);
		if (cmds_.size() == size_pack_) {
			flush_commands();
			cmds_.clear();
		}
	}
	else {
		flush_commands();		
		cmd_parser_->ChangeToDynamic();
	}
}

void StaticState::SetEndState(){
	flush_commands();
	cmds_.clear();
}

DynamicState::DynamicState(CommandParser* cmd_parser) : PackState(cmd_parser), brace_counter_(1) {}

void DynamicState::HandleCommand(const string& cmd) {
	if (cmd == "}") {
		--brace_counter_;
		if (brace_counter_ == 0) {
			flush_commands();			
			cmd_parser_->ChangeToStatic();
		}
	}
	else if (cmd == "{") {
		++brace_counter_;
	}
	else {
		add_cmd(cmd);
	}
}

void DynamicState::SetEndState(){
	cmds_.clear();
}

CommandParser::CommandParser(uint32_t size_static_pack, istream& in, ostream& out) : 
                                                is_end_stream_(false),
												is_enable_log_(false),
												 size_static_pack_(size_static_pack),
												  in_(in),
												   out_(out) {
	assert(size_static_pack_ > 0);
    pack_state_ = make_unique<StaticState>(this, size_static_pack_);
}

void CommandParser::ChangeToStatic() {
	pack_state_ = make_unique<StaticState>(this, size_static_pack_);
}

void CommandParser::ChangeToDynamic() {
	pack_state_ = make_unique<DynamicState>(this);	
}

void CommandParser::HandleCommand() {
	string str;
	getline(in_, str);

	if (str == END_STREAM) {
		pack_state_->SetEndState();
		is_end_stream_ = true;
		return;
	}

	pack_state_->HandleCommand(str);
}

ostream& CommandParser::GetOutputStream(){
	return out_;
}

istream& CommandParser::GetInputStream(){
	return in_;
}

bool CommandParser::IsEndStream() const{
	return is_end_stream_;
}

bool CommandParser::IsLogEnabled() const{
	return is_enable_log_;
}

void CommandParser::EnableLog(){
	is_enable_log_ = true;
}

void CommandParser::DisableLog(){
	is_enable_log_ = false;
}

const string CommandParser::END_STREAM = "EOF";