#include "command_parser.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>

using namespace std;

CommandParser::PackState::PackState() : next_state_(CommandParser::NextState::NONE) {}

void CommandParser::PackState::FlushCommands(ostream& out) {
	if (cmds_.empty())
		return;

	string res;

	res += "bulk: " + cmds_[0];
	for (size_t i = 1; i < cmds_.size(); i++) {
		res += ", " + cmds_[i];
	}

	out << res << endl;
}

CommandParser::NextState CommandParser::PackState::GetNextState() const {
	return next_state_;
}

string CommandParser::PackState::GetFileName() const {
	ostringstream name_file;
	name_file << time_creation_ << ".log";
	return name_file.str();
}

void CommandParser::PackState::HandleCommand(const string& cmd) {
	if (cmds_.empty())
		time_creation_ = chrono::system_clock::to_time_t(chrono::system_clock::now());

	cmds_.push_back(cmd);
}

CommandParser::StaticState::StaticState(uint32_t size_pack) : size_pack_(size_pack) {
	cmds_.reserve(size_pack);
}

void CommandParser::StaticState::HandleCommand(const string& cmd) {
	if (cmd != "{") {
		PackState::HandleCommand(cmd);
		if (cmds_.size() == size_pack_) {
			next_state_ = CommandParser::NextState::STATIC;
		}
	}
	else {
		next_state_ = CommandParser::NextState::DYNAMIC;
	}
}

void CommandParser::StaticState::SetEndStream() {
	next_state_ = CommandParser::NextState::STATIC;
}

void CommandParser::DynamicState::SetEndStream() {
	next_state_ = CommandParser::NextState::STATIC;
	cmds_.clear();
}

CommandParser::DynamicState::DynamicState() : brace_counter_(1) {}

void CommandParser::DynamicState::HandleCommand(const string& cmd) {
	if (cmd == "}") {
		--brace_counter_;
		if (brace_counter_ == 0) {
			next_state_ = CommandParser::NextState::STATIC;
		}
	}
	else if (cmd == "{") {
		++brace_counter_;
	}
	else {
		PackState::HandleCommand(cmd);
	}
}

void CommandParser::change_to_static() {
	pack_state_ = make_unique<StaticState>(size_static_pack_);
}

void CommandParser::change_to_dynamic() {
	pack_state_ = make_unique<DynamicState>();
}

CommandParser::CommandParser(uint32_t size_static_pack, istream& in, ostream& out) :
	is_enable_log_(false),
	size_static_pack_(size_static_pack),
	in_(in),
	out_(out)

{
	change_to_static();
}

void CommandParser::HandleCommand() {
	if (IsEndStream())
		return;

	string str;
	getline(in_, str);

	if (IsEndStream()) {
		pack_state_->SetEndStream();
		pack_state_->FlushCommands(out_);
		if (is_enable_log_) {
			ofstream out_file(pack_state_->GetFileName());
			pack_state_->FlushCommands(out_file);
		}

		return;
	}

	pack_state_->HandleCommand(str);

	if (pack_state_->GetNextState() != CommandParser::NextState::NONE) {
		pack_state_->FlushCommands(out_);

		if (is_enable_log_) {
			ofstream out_file(pack_state_->GetFileName());
			pack_state_->FlushCommands(out_file);
		}

		if (pack_state_->GetNextState() == CommandParser::NextState::STATIC) {
			change_to_static();
		}
		else if (pack_state_->GetNextState() == CommandParser::NextState::DYNAMIC) {
			change_to_dynamic();
		}
	}
}

bool CommandParser::IsEndStream() const {
	return in_.eof();
}

void CommandParser::EnableLog() {
	is_enable_log_ = true;
}

void CommandParser::DisableLog() {
	is_enable_log_ = false;
}
