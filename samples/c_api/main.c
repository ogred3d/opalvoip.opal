/*
 * main.c
 *
 * An example of the "C" interface to OPAL
 *
 * Open Phone Abstraction Library
 *
 * Copyright (c) 2008 Vox Lucida
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is Open Phone Abstraction Library.
 *
 * The Initial Developer of the Original Code is Vox Lucida (Robert Jongbloed)
 *
 * Contributor(s): ______________________________________.
 *
 * $Revision$
 * $Author$
 * $Date$
 */

#define _CRT_NONSTDC_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <limits.h>
#include <opal.h>


#define LOCAL_MEDIA 0


#if defined(_WIN32)

  #include <windows.h>

  #ifdef _DEBUG
    #define OPAL_DLL "OPALd.DLL"
  #else
    #define OPAL_DLL "OPAL.DLL"
  #endif

  #define OPEN_LIBRARY(name)             LoadLibrary(name)
  #define GET_LIBRARY_FUNCTION(dll, fn)  GetProcAddress(dll, fn)

  HINSTANCE hDLL;

  #define snprintf _snprintf

#else // _WIN32

  #include <memory.h>
  #include <dlfcn.h>

  #define OPAL_DLL "libopal.so"

  #define OPEN_LIBRARY(name)             dlopen(name, RTLD_NOW|RTLD_GLOBAL)
  #define GET_LIBRARY_FUNCTION(dll, fn)  dlsym(dll, (const char *)(fn));

  void * hDLL;

#endif // _WIN32

OpalInitialiseFunction  InitialiseFunction;
OpalShutDownFunction    ShutDownFunction;
OpalGetMessageFunction  GetMessageFunction;
OpalSendMessageFunction SendMessageFunction;
OpalFreeMessageFunction FreeMessageFunction;
OpalHandle              hOPAL;

char * CurrentCallToken;
char * HeldCallToken;
char * PlayScript;



OpalMessage * MySendCommand(OpalMessage * command, const char * errorMessage)
{
  OpalMessage * response;
  if ((response = SendMessageFunction(hOPAL, command)) == NULL)
    return NULL;
  if (response->m_type != OpalIndCommandError)
    return response;

  if (response->m_param.m_commandError == NULL || *response->m_param.m_commandError == '\0')
    printf("%s.\n", errorMessage);
  else
    printf("%s: %s\n", errorMessage, response->m_param.m_commandError);

  FreeMessageFunction(response);

  return NULL;
}


#if LOCAL_MEDIA

#ifdef _MSC_VER
#pragma warning(disable:4100)
#endif

int MyReadMediaData(const char * token, const char * id, const char * format, void * userData, void * data, int size)
{
  static FILE * file = NULL;
  if (file == NULL) {
    if (strcmp(format, "PCM-16") == 0)
      file = fopen("ogm.wav", "rb");
    printf("Reading %s media for stream %s on call %s\n", format, id, token);
  }

  if (file != NULL)
    return fread(data, 1, size, file);

  memset(data, 0, size);
  return size;
}


int MyWriteMediaData(const char * token, const char * id, const char * format, void * userData, void * data, int size)
{
  static FILE * file = NULL;
  if (file == NULL) {
    char name[100];
    sprintf(name, "Media-%s-%s.%s", token, id, format);
    file = fopen(name, "wb");
    if (file == NULL) {
      printf("Could not create media output file \"%s\"\n", name);
      return -1;
    }
    printf("Writing %s media for stream %s on call %s\n", format, id, token);
  }

  return fwrite(data, 1, size, file);
}

#ifdef _MSC_VER
#pragma warning(default:4100)
#endif

#endif // LOCAL_MEDIA


int InitialiseOPAL()
{
  OpalMessage   command;
  OpalMessage * response;
  unsigned      version;

  static const char OPALOptions[] = OPAL_PREFIX_H323  " "
                                    OPAL_PREFIX_SIP   " "
                                    OPAL_PREFIX_IAX2  " "
#if LOCAL_MEDIA
                                    OPAL_PREFIX_LOCAL
#else
                                    OPAL_PREFIX_PCSS
#endif
                                                      " "
                                    OPAL_PREFIX_IVR
                                    " TraceLevel=4 TraceFile=debugstream";


  if ((hDLL = OPEN_LIBRARY(OPAL_DLL)) == NULL) {
    fprintf(stderr, "Could not file %s\n", OPAL_DLL);
    return 0;
  }

  InitialiseFunction  = (OpalInitialiseFunction )GET_LIBRARY_FUNCTION(hDLL, OPAL_INITIALISE_FUNCTION  );
  ShutDownFunction    = (OpalShutDownFunction   )GET_LIBRARY_FUNCTION(hDLL, OPAL_SHUTDOWN_FUNCTION    );
  GetMessageFunction  = (OpalGetMessageFunction )GET_LIBRARY_FUNCTION(hDLL, OPAL_GET_MESSAGE_FUNCTION );
  SendMessageFunction = (OpalSendMessageFunction)GET_LIBRARY_FUNCTION(hDLL, OPAL_SEND_MESSAGE_FUNCTION);
  FreeMessageFunction = (OpalFreeMessageFunction)GET_LIBRARY_FUNCTION(hDLL, OPAL_FREE_MESSAGE_FUNCTION);

  if (InitialiseFunction  == NULL ||
      ShutDownFunction    == NULL ||
      GetMessageFunction  == NULL ||
      SendMessageFunction == NULL ||
      FreeMessageFunction == NULL) {
    fputs("OPAL.DLL is invalid\n", stderr);
    return 0;
  }


  ///////////////////////////////////////////////
  // Initialisation

  version = OPAL_C_API_VERSION;
  if ((hOPAL = InitialiseFunction(&version, OPALOptions)) == NULL) {
    fputs("Could not initialise OPAL\n", stderr);
    return 0;
  }

#if 0
  // Test shut down and re-initialisation
  ShutDownFunction(hOPAL);
  if ((hOPAL = InitialiseFunction(&version, OPALOptions)) == NULL) {
    fputs("Could not re-initialise OPAL\n", stderr);
    return 0;
  }
#endif


  // General options
  memset(&command, 0, sizeof(command));
  command.m_type = OpalCmdSetGeneralParameters;
  //command.m_param.m_general.m_audioRecordDevice = "Camera Microphone (2- Logitech";
  command.m_param.m_general.m_autoRxMedia = command.m_param.m_general.m_autoTxMedia = "audio";
  command.m_param.m_general.m_stunServer = "stun.voxgratia.org";
  command.m_param.m_general.m_mediaMask = "RFC4175*";

#if LOCAL_MEDIA
  command.m_param.m_general.m_mediaReadData = MyReadMediaData;
  command.m_param.m_general.m_mediaWriteData = MyWriteMediaData;
  command.m_param.m_general.m_mediaDataHeader = OpalMediaDataPayloadOnly;
#endif

  if ((response = MySendCommand(&command, "Could not set general options")) == NULL)
    return 0;

  FreeMessageFunction(response);

  // Options across all protocols
  memset(&command, 0, sizeof(command));
  command.m_type = OpalCmdSetProtocolParameters;

  command.m_param.m_protocol.m_userName = "robertj";
  command.m_param.m_protocol.m_displayName = "Robert Jongbloed";
  command.m_param.m_protocol.m_interfaceAddresses = "*";

  if ((response = MySendCommand(&command, "Could not set protocol options")) == NULL)
    return 0;

  FreeMessageFunction(response);

  memset(&command, 0, sizeof(command));
  command.m_type = OpalCmdSetProtocolParameters;

  command.m_param.m_protocol.m_prefix = "sip";
  command.m_param.m_protocol.m_defaultOptions = "PRACK-Mode=0\nInitial-Offer=false";

  if ((response = MySendCommand(&command, "Could not set SIP options")) == NULL)
    return 0;

  FreeMessageFunction(response);

  return 1;
}


static void HandleMessages(unsigned timeout)
{
  OpalMessage command;
  OpalMessage * response;
  OpalMessage * message;
    

  while ((message = GetMessageFunction(hOPAL, timeout)) != NULL) {
    switch (message->m_type) {
      case OpalIndRegistration :
        switch (message->m_param.m_registrationStatus.m_status) {
          case OpalRegisterRetrying :
            printf("Trying registration to %s.\n", message->m_param.m_registrationStatus.m_serverName);
            break;
          case OpalRegisterRestored :
            printf("Registration of %s restored.\n", message->m_param.m_registrationStatus.m_serverName);
            break;
          case OpalRegisterSuccessful :
            printf("Registration of %s successful.\n", message->m_param.m_registrationStatus.m_serverName);
            break;
          case OpalRegisterRemoved :
            printf("Unregistered %s.\n", message->m_param.m_registrationStatus.m_serverName);
            break;
          case OpalRegisterFailed :
            if (message->m_param.m_registrationStatus.m_error == NULL ||
                message->m_param.m_registrationStatus.m_error[0] == '\0')
              printf("Registration of %s failed.\n", message->m_param.m_registrationStatus.m_serverName);
            else
              printf("Registration of %s error: %s\n",
                     message->m_param.m_registrationStatus.m_serverName,
                     message->m_param.m_registrationStatus.m_error);
        }
        break;

      case OpalIndLineAppearance :
        switch (message->m_param.m_lineAppearance.m_state) {
          case OpalLineIdle :
            printf("Line %s available.\n", message->m_param.m_lineAppearance.m_line);
            break;
          case OpalLineTrying :
            printf("Line %s in use.\n", message->m_param.m_lineAppearance.m_line);
            break;
          case OpalLineProceeding :
            printf("Line %s calling.\n", message->m_param.m_lineAppearance.m_line);
            break;
          case OpalLineRinging :
            printf("Line %s ringing.\n", message->m_param.m_lineAppearance.m_line);
            break;
          case OpalLineConnected :
            printf("Line %s connected.\n", message->m_param.m_lineAppearance.m_line);
            break;
          case OpalLineSubcribed :
            printf("Line %s subscription successful.\n", message->m_param.m_lineAppearance.m_line);
            break;
          case OpalLineUnsubcribed :
            printf("Unsubscribed line %s.\n", message->m_param.m_lineAppearance.m_line);
            break;
        }
        break;

      case OpalIndIncomingCall :
        printf("Incoming call from \"%s\", \"%s\" to \"%s\", handled by \"%s\".\n",
               message->m_param.m_incomingCall.m_remoteDisplayName,
               message->m_param.m_incomingCall.m_remoteAddress,
               message->m_param.m_incomingCall.m_calledAddress,
               message->m_param.m_incomingCall.m_localAddress);
        if (CurrentCallToken == NULL) {
          memset(&command, 0, sizeof(command));
          command.m_type = OpalCmdAnswerCall;
          command.m_param.m_answerCall.m_callToken = message->m_param.m_incomingCall.m_callToken;
          command.m_param.m_answerCall.m_overrides.m_userName = "test123";
          command.m_param.m_answerCall.m_overrides.m_displayName = "Test Called Party";
          if ((response = MySendCommand(&command, "Could not answer call")) != NULL)
            FreeMessageFunction(response);
        }
        else {
          memset(&command, 0, sizeof(command));
          command.m_type = OpalCmdClearCall;
          command.m_param.m_clearCall.m_callToken = message->m_param.m_incomingCall.m_callToken;
          command.m_param.m_clearCall.m_reason = OpalCallEndedByLocalBusy;
          if ((response = MySendCommand(&command, "Could not refuse call")) != NULL)
            FreeMessageFunction(response);
        }
        break;

      case OpalIndProceeding :
        puts("Proceeding.\n");
        break;

      case OpalIndAlerting :
        puts("Ringing.\n");
        break;

      case OpalIndEstablished :
        puts("Established.\n");

        if (PlayScript != NULL) {
          printf("Playing %s\n", PlayScript);

          memset(&command, 0, sizeof(command));
          command.m_type = OpalCmdTransferCall;
          command.m_param.m_callSetUp.m_callToken = CurrentCallToken;
          command.m_param.m_callSetUp.m_partyA = "pc:";
          command.m_param.m_callSetUp.m_partyB = PlayScript;
          if ((response = MySendCommand(&command, "Could not start playing")) != NULL)
            FreeMessageFunction(response);
        }
        break;

      case OpalIndMediaStream :
        printf("Media stream %s %s using %s.\n",
               message->m_param.m_mediaStream.m_type,
               message->m_param.m_mediaStream.m_state == OpalMediaStateOpen ? "opened" : "closed",
               message->m_param.m_mediaStream.m_format);
        break;

      case OpalIndUserInput :
        printf("User Input: %s.\n", message->m_param.m_userInput.m_userInput);
        break;

      case OpalIndCallCleared :
        if (message->m_param.m_callCleared.m_reason == NULL)
          puts("Call cleared.\n");
        else
          printf("Call cleared: %s\n", message->m_param.m_callCleared.m_reason);
        break;

      default :
        break;
    }

    FreeMessageFunction(message);
  }
}


int DoCall(const char * from, const char * to)
{
  // Example cmd line: call 612@ekiga.net
  OpalMessage command;
  OpalMessage * response;


  printf("Calling %s\n", to);

  memset(&command, 0, sizeof(command));
  command.m_type = OpalCmdSetUpCall;
  command.m_param.m_callSetUp.m_partyA = from;
  command.m_param.m_callSetUp.m_partyB = to;
  command.m_param.m_callSetUp.m_overrides.m_displayName = "Test Calling Party";
  if ((response = MySendCommand(&command, "Could not make call")) == NULL)
    return 0;

  CurrentCallToken = strdup(response->m_param.m_callSetUp.m_callToken);
  FreeMessageFunction(response);
  return 1;
}


int DoMute(int on)
{
  // Example cmd line: mute 612@ekiga.net
  OpalMessage command;
  OpalMessage * response;


  printf("Mute %s\n", on ? "on" : "off");

  memset(&command, 0, sizeof(command));
  command.m_type = OpalCmdMediaStream;
  command.m_param.m_mediaStream.m_callToken = CurrentCallToken;
  command.m_param.m_mediaStream.m_type = "audio out";
  command.m_param.m_mediaStream.m_state = on ? OpalMediaStatePause : OpalMediaStateResume;
  if ((response = MySendCommand(&command, "Could not mute call")) == NULL)
    return 0;

  FreeMessageFunction(response);
  return 1;
}


int DoHold()
{
  // Example cmd line: hold 612@ekiga.net
  OpalMessage command;
  OpalMessage * response;


  printf("Hold\n");

  memset(&command, 0, sizeof(command));
  command.m_type = OpalCmdHoldCall;
  command.m_param.m_callToken = CurrentCallToken;
  if ((response = MySendCommand(&command, "Could not hold call")) == NULL)
    return 0;

  HeldCallToken = CurrentCallToken;
  CurrentCallToken = NULL;

  FreeMessageFunction(response);
  return 1;
}


int DoTransfer(const char * to)
{
  // Example cmd line: transfer fred@10.0.1.11 noris@10.0.1.15
  OpalMessage command;
  OpalMessage * response;


  printf("Transferring to %s\n", to);

  memset(&command, 0, sizeof(command));
  command.m_type = OpalCmdTransferCall;
  command.m_param.m_callSetUp.m_partyB = to;
  command.m_param.m_callSetUp.m_callToken = CurrentCallToken;
  if ((response = MySendCommand(&command, "Could not transfer call")) == NULL)
    return 0;

  FreeMessageFunction(response);
  return 1;
}


int DoRegister(const char * aor, const char * pwd)
{
  // Example cmd line: register robertj@ekiga.net secret
  OpalMessage command;
  OpalMessage * response;
  char * colon;


  printf("Registering %s\n", aor);

  memset(&command, 0, sizeof(command));
  command.m_type = OpalCmdRegistration;

  if ((colon = strchr(aor, ':')) == NULL) {
    command.m_param.m_registrationInfo.m_protocol = "h323";
    command.m_param.m_registrationInfo.m_identifier = aor;
  }
  else {
    *colon = '\0';
    command.m_param.m_registrationInfo.m_protocol = aor;
    command.m_param.m_registrationInfo.m_identifier = colon+1;
  }

  command.m_param.m_registrationInfo.m_password = pwd;
  command.m_param.m_registrationInfo.m_timeToLive = 300;
  if ((response = MySendCommand(&command, "Could not register endpoint")) == NULL)
    return 0;

  FreeMessageFunction(response);
  return 1;
}


int DoSubscribe(const char * package, const char * aor, const char * from)
{
  // Example cmd line: subscribe "dialog;sla;ma" 1501@192.168.1.32 1502@192.168.1.32
  OpalMessage command;
  OpalMessage * response;


  printf("Susbcribing %s\n", aor);

  memset(&command, 0, sizeof(command));
  command.m_type = OpalCmdRegistration;
  command.m_param.m_registrationInfo.m_protocol = "sip";
  command.m_param.m_registrationInfo.m_identifier = aor;
  command.m_param.m_registrationInfo.m_hostName = from;
  command.m_param.m_registrationInfo.m_eventPackage = package;
  command.m_param.m_registrationInfo.m_timeToLive = 300;
  if ((response = MySendCommand(&command, "Could not subscribe")) == NULL)
    return 0;

  FreeMessageFunction(response);
  return 1;
}


int DoRecord(const char * to, const char * file)
{
  // Example cmd line: call 612@ekiga.net
  OpalMessage command;
  OpalMessage * response;


  printf("Calling %s\n", to);

  memset(&command, 0, sizeof(command));
  command.m_type = OpalCmdSetUpCall;
  command.m_param.m_callSetUp.m_partyB = to;
  if ((response = MySendCommand(&command, "Could not make call")) == NULL)
    return 0;

  CurrentCallToken = strdup(response->m_param.m_callSetUp.m_callToken);
  FreeMessageFunction(response);

  printf("Recording %s\n", file);

  memset(&command, 0, sizeof(command));
  command.m_type = OpalCmdStartRecording;
  command.m_param.m_recording.m_callToken = CurrentCallToken;
  command.m_param.m_recording.m_file = file;
  command.m_param.m_recording.m_channels = 2;
  if ((response = MySendCommand(&command, "Could not start recording")) == NULL)
    return 0;

  return 1;
}


int DoPlay(const char * to, const char * file)
{
  // Example cmd line: call 612@ekiga.net
  OpalMessage command;
  OpalMessage * response;


  printf("Playing %s to %s\n", file, to);

  PlayScript = (char *)malloc(1000);
  snprintf(PlayScript, 999,
           "ivr:<?xml version=\"1.0\"?>"
           "<vxml version=\"1.0\">"
             "<form id=\"PlayFile\">"
               "<audio src=\"%s\"/>"
             "</form>"
           "</vxml>", file);

  memset(&command, 0, sizeof(command));
  command.m_type = OpalCmdSetUpCall;
  command.m_param.m_callSetUp.m_partyB = to;
  if ((response = MySendCommand(&command, "Could not make call")) == NULL)
    return 0;

  CurrentCallToken = strdup(response->m_param.m_callSetUp.m_callToken);
  FreeMessageFunction(response);

  return 1;
}


typedef enum
{
  OpListen,
  OpCall,
  OpMute,
  OpHold,
  OpTransfer,
  OpConsult,
  OpRegister,
  OpSubscribe,
  OpRecord,
  OpPlay,
  NumOperations
} Operations;

static const char * const OperationNames[NumOperations] =
  { "listen", "call", "mute", "hold", "transfer", "consult", "register", "subscribe", "record", "play" };

static int const RequiredArgsForOperation[NumOperations] =
  { 2, 3, 3, 3, 4, 4, 3, 3, 3, 3 };


static Operations GetOperation(const char * name)
{
  Operations op;

  for (op = OpListen; op < NumOperations; op++) {
    if (strcmp(name, OperationNames[op]) == 0)
      break;
  }

  return op;
}


int main(int argc, char * argv[])
{
  Operations operation;

  
  if (argc < 2 || (operation = GetOperation(argv[1])) == NumOperations || argc < RequiredArgsForOperation[operation]) {
    Operations op;
    fputs("usage: c_api { ", stderr);
    for (op = OpListen; op < NumOperations; op++) {
      if (op > OpListen)
        fputs(" | ", stderr);
      fputs(OperationNames[op], stderr);
    }
    fputs(" } [ A-party [ B-party | file ] ]\n", stderr);
    return 1;
  }

  puts("Initialising.\n");

  if (!InitialiseOPAL())
    return 1;

  switch (operation) {
    case OpListen :
      puts("Listening.\n");
      HandleMessages(60000);
      break;

    case OpCall :
      if (argc > 3) {
        if (!DoCall(argv[2], argv[3]))
          break;
      } else {
        if (!DoCall(NULL, argv[2]))
          break;
      }
      HandleMessages(15000);
      break;

    case OpMute :
      if (!DoCall(NULL, argv[2]))
        break;
      HandleMessages(15000);
      if (!DoMute(1))
        break;
      HandleMessages(15000);
      if (!DoMute(0))
        break;
      HandleMessages(15000);
      break;

    case OpHold :
      if (!DoCall(NULL, argv[2]))
        break;
      HandleMessages(15000);
      if (!DoHold())
        break;
      HandleMessages(15000);
      break;

    case OpTransfer :
      if (!DoCall(NULL, argv[2]))
        break;
      HandleMessages(15000);
      if (!DoTransfer(argv[3]))
        break;
      HandleMessages(15000);
      break;

    case OpConsult :
      if (!DoCall(NULL, argv[2]))
        break;
      HandleMessages(15000);
      if (!DoHold())
        break;
      HandleMessages(15000);
      if (!DoCall(NULL, argv[3]))
        break;
      HandleMessages(15000);
      if (!DoTransfer(HeldCallToken))
        break;
      HandleMessages(15000);
      break;

    case OpRegister :
      if (!DoRegister(argv[2], argv[3]))
        break;
      HandleMessages(15000);
      break;

    case OpSubscribe :
      if (!DoSubscribe(argv[2], argv[3], argv[4]))
        break;
      HandleMessages(INT_MAX); // More or less forever
      break;

    case OpRecord :
      if (!DoRecord(argv[2], argv[3]))
        break;
      HandleMessages(INT_MAX); // More or less forever
      break;

    case OpPlay :
      if (!DoPlay(argv[2], argv[3]))
        break;
      HandleMessages(INT_MAX); // More or less forever
      break;

    default :
      break;
  }

  puts("Exiting.\n");

  ShutDownFunction(hOPAL);
  return 0;
}



// End of File ///////////////////////////////////////////////////////////////
