#pragma once

#include<ctime>
#include<vector>
#include<string>
#include<memory>
#include<iostream>

class CommandParser;

class CommandParser {
	enum class NextState { NONE, STATIC, DYNAMIC };

	class PackState {
		time_t time_creation_;
	protected:
		std::vector<std::string> cmds_;
		NextState next_state_;
	public:
		PackState();
		void FlushCommands(std::ostream& out);
		NextState GetNextState() const;
		std::string    GetFileName() const;

		virtual void HandleCommand(const std::string& cmd);
		virtual void SetEndStream() = 0;
		virtual ~PackState() {}
	};

	class StaticState : public PackState {
		uint32_t size_pack_;
	public:
		StaticState(uint32_t size_pack);
		void HandleCommand(const std::string& cmd) override;
		void SetEndStream() override;
	};

	class DynamicState : public PackState {
		uint32_t brace_counter_;
	public:
		DynamicState();
		void HandleCommand(const std::string& cmd) override;
		void SetEndStream() override;
	};
private:

	bool is_enable_log_;
	uint32_t size_static_pack_;
	std::istream& in_;
	std::ostream& out_;

	std::unique_ptr<PackState> pack_state_;

	void change_to_static();
	void change_to_dynamic();
public:

	CommandParser(uint32_t size_static_pack, std::istream& in, std::ostream& out);

	void HandleCommand();
	bool IsEndStream() const;
	void EnableLog();
	void DisableLog();
};
