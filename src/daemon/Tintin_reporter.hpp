/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tintin_reporter.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/17 14:01:31 by kiroussa          #+#    #+#             */
/*   Updated: 2025/07/17 14:35:43 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#if MATT_MODE
#pragma once

#include <string>

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
	private:
		int fd;
		void report(std::string const& type, std::string const& message);
};
#endif // MATT_MODE
