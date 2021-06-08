#pragma once

#include<ctime>
#include<vector>
#include<string>
#include<memory>

class CommandParser;

class PackState {
	time_t time_creation;
protected:
	std::vector<std::string> cmds_;
	CommandParser* cmd_parser_;
	void flush_commands();
	void add_cmd(const std::string& cmd);

public:
	PackState(CommandParser* pack);
	virtual void HandleCommand(const std::string& cmd) = 0;
	virtual void SetEndState() = 0;	
};

class StaticState : public PackState {
	uint32_t size_pack_;
public:
	StaticState(CommandParser* pack, uint32_t size_pack);

	void HandleCommand(const std::string& cmd) override;
	void SetEndState() override;	
};

class DynamicState : public PackState {
	uint32_t brace_counter_;
public:
	DynamicState(CommandParser* pack);

	void HandleCommand(const std::string& cmd) override;
	void SetEndState() override;	
};

class CommandParser {	
	bool is_end_stream_;
	bool is_enable_log_;
	uint32_t size_static_pack_;
    std::istream& in_;
    std::ostream& out_;

	std::unique_ptr<PackState> pack_state_;
	static const std::string END_STREAM;

public:
	friend class PackState;
	CommandParser(uint32_t size_static_pack, std::istream& in, std::ostream& out);

	void ChangeToStatic();
	void ChangeToDynamic();

    std::istream& GetInputStream();
    std::ostream& GetOutputStream();

	void HandleCommand();
	bool IsEndStream() const;
	bool IsLogEnabled() const;
	void EnableLog();
	void DisableLog();
};