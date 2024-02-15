/**
 * "COMMON/WINSOCK2.H"
 */

/**************************************************************/
/* "WinSock2" defines                                         */
/**************************************************************/

#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)

#define SOCK_DGRAM  2

#define IPPROTO_UDP  17

#define INADDR_ANY  ((uint32_t) 0x00000000)

/* internetwork: UDP, TCP, etc. */
#define AF_INET  2

#define INET_ADDRSTRLEN  22

#define WSADESCRIPTION_LEN  256
#define WSASYS_STATUS_LEN   128

#define WINSOCK_VERSION  0x0202

/**************************************************************/
/* "WinSock2" types                                           */
/**************************************************************/

typedef uint16_t ADDRESS_FAMILY;

typedef uintptr_t SOCKET;

/**
 * IPv4 Internet address
 */
typedef struct in_addr IN_ADDR;

struct in_addr
{
    union
    {
        struct { uint8_t s_b1, s_b2, s_b3, s_b4; } S_un_b;
        struct { uint16_t s_w1,s_w2; } S_un_w;
        uint32_t S_addr;
    }
    S_un;

    #define s_addr  S_un.S_addr
};

/**
 * IPv4 Socket address, Internet style
 */
typedef struct sockaddr_in SOCKADDR_IN;

struct sockaddr_in
{
    ADDRESS_FAMILY sin_family;
    uint16_t       sin_port;
    IN_ADDR        sin_addr;
    int8_t         sin_zero[8];
};

/**
 * Structure used to store most addresses.
 */
typedef struct sockaddr SOCKADDR;

struct sockaddr
{
    ADDRESS_FAMILY sa_family;
    int8_t         sa_data[14];
};

typedef struct WSAData WSADATA;

struct WSAData
{
    uint16_t wVersion;
    uint16_t wHighVersion;
    char     szDescription[WSADESCRIPTION_LEN+1];
    char     szSystemStatus[WSASYS_STATUS_LEN+1];
    uint16_t iMaxSockets;
    uint16_t iMaxUdpDg;
    char     *lpVendorInfo;
};

/**************************************************************/
/* "WS2_32.LIB" procedures                                    */
/**************************************************************/

int
__stdcall
WSAStartup(
    uint16_t wVersionRequested,
    WSADATA  *lpWSAData);

int
__stdcall
WSAGetLastError(void);

int
__stdcall
WSACleanup(void);

/**
 * Converts Network Address
 * from Standard Text Form
 * to Numeric Binary Form
 */
int
__stdcall
inet_pton(
    int32_t    family,
    char const *text,
    void       *inetAddr);

/**
 * Converts Network Address
 * from Numeric Binary Form
 * to Standard Text Form
 */
int
__stdcall
inet_ntop(
    int32_t    family,
    const void *inetAddr,
    char       *text,
    void       *n);

/**
 * Convert UInt16
 * from host byte order (little-endian)
 * to network byte order (big-endian).
 */
uint16_t
__stdcall
htons(
    uint16_t hostshort);

/**
 * Convert UInt16
 * from host byte order (little-endian)
 * to network byte order (big-endian).
 */
uint16_t
__stdcall
ntohs(
    uint16_t netshort);

SOCKET
__stdcall
socket(
    int af,
    int type,
    int protocol);

int
__stdcall
closesocket(
    SOCKET s);

int
__stdcall
recvfrom(
    SOCKET   s,
    char     *buf,
    int      len,
    int      flags,
    SOCKADDR *from,
    int      *fromlen);


int
__stdcall
sendto(
    SOCKET         s,
    char const     *buf,
    int            len,
    int            flags,
    SOCKADDR const *to,
    int            tolen);

int
__stdcall
bind(
    SOCKET         s,
    SOCKADDR const *addr,
    int            namelen);

/**************************************************************/
