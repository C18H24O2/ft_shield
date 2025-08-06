/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tintin_reporter.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/17 14:01:31 by kiroussa          #+#    #+#             */
/*   Updated: 2025/08/06 20:02:00 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#if MATT_MODE
#pragma once

#include <string>
#include <shield.h>

class Tintin_reporter
{
	public:
		Tintin_reporter();
		~Tintin_reporter();
		Tintin_reporter(const Tintin_reporter &) = delete;
		Tintin_reporter& operator=(const Tintin_reporter &) = delete;

		int init(std::string const& parent_dir, std::string const& path);

		void info(std::string const& message);
		void error(std::string const& message);

		void report(std::string const& type, std::string const& message);
	private:
		int fd;
};
#endif // MATT_MODE
