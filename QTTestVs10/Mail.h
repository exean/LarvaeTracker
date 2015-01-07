/**
 *  @file Mail.h
 *
 *	@section DESCRIPTION
 *	Singleton class for sending mails.
 *	Implemented through plugins.
 */
#pragma once

#include <iostream>
#include <string>
#include <QString>
#include <QDir>
#include <windows.h>
#include "GUISettings.h"

/**
*	Creator-Methods of plugins
**/
typedef bool * (*SendMailFunc)(
	const char*	username,
	const char*	password,
	const char*	sendername,
	const char*	sendermail,
	const char*	recipient,
	const char*	subject,
	const char*	message,		
	const char*	smtpserver,
	const int		port,
	const char*	attachmentPath);
/**
 *	Singleton class for sending mails.
 *	Implemented through plugins.
 */
class Mail
{
public:
	static Mail* Instance();
	~Mail(void);

	/**
	*	Send a mail with specified valued
	**/
	bool sendMail(
		const char*	subject,
		const char*	message,
		const char*	attachmentFilePath = 0,
		const char*	recipient	= 0,//0 b/c default value exists
		const char*	username	= 0,
		const char*	password	= 0,
		const char*	address		= 0,
		const char*	server		= 0,
		int				port		= -1);

private:
	Mail(void);
	static Mail* mInstance;

	void setup();
	SendMailFunc mMailFunc;
};

