#include <shield/commands.hpp>
#include <sstream>

int shield_cmd_help(Client *client, const char *args)
{
	if (!args | !*args)
	{
		for (int i = 0; COMMANDS[i].command != NULL; ++i)
		{
			client->output_buffer.append(COMMANDS[i].usage);
			client->output_buffer.append(": ");
			client->output_buffer.append(COMMANDS[i].description);
			client->output_buffer.push_back('\n');
		}
		return (0);
	}

	std::string option;
	std::istringstream argstream(args);
	argstream >> option;
	for (int i = 0; COMMANDS[i].command != NULL; ++i)
	{
		if (option == COMMANDS[i].command)
		{
			client->output_buffer.append("Usage: ");
			client->output_buffer.append(COMMANDS[i].usage);
			client->output_buffer.push_back('\n');

			client->output_buffer.append("Description: ");
			client->output_buffer.append(COMMANDS[i].description);
			client->output_buffer.push_back('\n');

			return 0;
		}
	}

	client->output_buffer.append("Command not found: ");
	client->output_buffer.append(option);
	client->output_buffer.push_back('\n');

	return (-1);
}

int	shield_cmd_shell(Client *client, const char *args)
{
	return (0);
}

int	shield_cmd_screenshot(Client *client, const char *args)
{
	return (0);
}

int	shield_cmd_get(Client *client, const char *args)
{
	return (0);
}

int	shield_cmd_put(Client *client, const char *args)
{
	return(0);
}
