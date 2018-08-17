////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WirelessErrorType
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef WIRELESSERRORTYPE_H
#define WIRELESSERRORTYPE_H

#ifndef MAX_ERROR_STRING
#define MAX_ERROR_STRING                               512
#endif

#define ERRORTYPE_SUCCESS                              (LRESULT)0L
//no Internet Online
#define NO_INTENET_ONLINE                              (LRESULT)21L
//No wireless device was found
#define NO_WIRELESS_DEVICE                             (LRESULT)22L
//Operating systems are not supported
#define SYSTEM_NOT_SUPPORT                             (LRESULT)23L
//No wireless network adapter was found
#define NO_WIRELESS_ADAPTER                            (LRESULT)24L
//Wlan API version is low not support
#define WLAN_API_VERLOW_NOT_SUPPORT                    (LRESULT)25L
#define WLAN_HOSTED_CANNOT_INIT                        (LRESULT)26L
#define INETSHARD_CONNECTION_ERROR                     (LRESULT)27L
#define CLOSE_WIFIHOSTED_ERROR                         (LRESULT)28L
#define SET_SECONDKEY_ERROR                            (LRESULT)29L
#define WLANHOSTED_FORCE_START_ERROR                   (LRESULT)30L
#define HOSTEDNETWORK_DISABLE_BY_GROUPPOLICY           (LRESULT)31L
#define WLANHANDLEOPEN_ALLOCATE_MEMORY                 (LRESULT)32L
#define WLANHANDLEOPEN_ERROR_INVALID_PARAMETER         (LRESULT)33L
#define WHO_ERROR_REMOTE_SESSION_LIMIT_EXCEEDED        (LRESULT)34L
#define SERVICE_NOT_START                              (LRESULT)35L
#define COM_COMPONENT_FAILED_TO_INITIALIZE             (LRESULT)36L
#define SC_HANDLE_OPEN_ERROR                           (LRESULT)37L
#define WLANSVC_START_ERROR                            (LRESULT)38L
#define WLANSVC_STOP_ERROR                          (LRESULT)39L
//Get Error Message String .
void WINAPI GetErrorMessageString(LRESULT hr, PWSTR pstr);

#endif