#include "Mail.h"

#pragma region Singleton
// Global static pointer used to ensure a single instance of the class.
Mail* Mail::mInstance = NULL; 

Mail* Mail::Instance()
{
	if (!mInstance)   // Only allow one instance of class to be generated.
	{
		mInstance = new Mail;
	}
	return mInstance;
}
#pragma endregion

Mail::Mail()
{
	this->setup();
}

Mail::~Mail()
{
}

bool Mail::sendMail(
		const char*	subject,
		const char*	message,
		const char*	attachmentFilePath,
		const char*	recipient,
		const char*	username,
		const char*	password,
		const char*	address,
		const char*	server,
		int				port)
{
	if(this->mMailFunc)
	{
		Gui::GUISettings * settings = Gui::GUISettings::Instance();
		return this->mMailFunc(
			username	? username	: settings->getMailUsername().toStdString().c_str(), 
			password	? password	: settings->getMailPassword().toStdString().c_str(),
			"PathFinder", 
			address		? address	: settings->getMailAddress().toStdString().c_str(),
			recipient	? recipient : settings->getMailDefaultRecipient().toStdString().c_str(),
			subject, 
			message, 
			server		? server	: settings->getMailServer().toStdString().c_str(),
			port < 0	? settings->getMailPort() : port,
			attachmentFilePath);
	}
	return false;
}

void Mail::setup()
{
	//Try to load mailing-library
	std::wstring cdll = (QString(".")+QDir::separator()+"Ressources"+QDir::separator()+"SmtpMailer.dll").toStdWString();
	HINSTANCE dllHandle = LoadLibrary(cdll.c_str());
	if(dllHandle)
	{
		this->mMailFunc = (SendMailFunc) ::GetProcAddress (dllHandle, "SendMail");			
	}
}