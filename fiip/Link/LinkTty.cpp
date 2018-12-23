#include "LinkTty.hpp"

#define FAIL -1
#define OK 0
static int speed_arr[] = {B115200, B57600, B38400, B19200, B9600, B4800,
                          B2400,   B1200,  B600,   B300,   B110};
static int name_arr[] = {115200, 57600, 38400, 19200, 9600, 4800,
                         2400,   1200,  600,   300,   110};

int set_speed(int fd, int speed) {
  unsigned int i;
  int status;
  struct termios options;
  if (tcgetattr(fd, &options) != 0) {
    perror("SetupSerial speed");
    return (FAIL);
  }
  options.c_iflag &= ~(INLCR | ICRNL | IGNCR);
  options.c_oflag &= ~(ONLCR | OCRNL);
  options.c_iflag &= ~(IXON);

  for (i = 0; i < sizeof(speed_arr) / sizeof(int); i++) {
    if (speed == name_arr[i]) {
      tcflush(fd, TCIOFLUSH); /*empty input cache*/

      cfsetispeed(&options, speed_arr[i]);
      cfsetospeed(&options, speed_arr[i]);

      status = tcsetattr(fd, TCSANOW, &options);
      if (status != 0)
        perror("tcsetattr fd");
      return (OK);
    }
    tcflush(fd, TCIOFLUSH); /*empty input cache*/
  }

  return (OK);
}

/**
 * *@brief 设置串口数据位，停止位和效验位
 * *@param fd 类型 int 打开的串口文件描述符
 * *@param databits 类型 int 数据位 取值 为 7 或者8
 * *@param stopbits 类型 int 停止位 取值为 1 或者2
 * *@param parity 类型 int 效验类型 取值为N,E,O,S
 * */
int set_parity(int fd, int databits, int stopbits, int parity) {
  struct termios options;
  if (tcgetattr(fd, &options) != 0) {
    perror("SetupSerial parity");
    return (FAIL);
  }
  options.c_cflag &= ~CSIZE;
  options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); /*Input*/
  options.c_oflag &= ~OPOST;                          /*Output*/

  switch (databits) {
    case 7:
      options.c_cflag |= CS7;
      break;
    case 8:
      options.c_cflag |= CS8;
      break;
    default:
      fprintf(stderr, "Unsupported data size\n");
      return (FAIL);
  }

  switch (parity) {
    case 'n':
    case 'N':
      options.c_cflag &= ~PARENB; /* Clear parity enable */
      options.c_iflag &= ~INPCK;  /* Enable parity checking */
      break;
    case 'o':
    case 'O':
      options.c_cflag |= (PARODD | PARENB); /* 设置为奇效验*/
      options.c_iflag |= INPCK;             /* Disnable parity checking */
      break;
    case 'e':
    case 'E':
      options.c_cflag |= PARENB;  /* Enable parity */
      options.c_cflag &= ~PARODD; /* 转换为偶效验*/
      options.c_iflag |= INPCK;   /* Disnable parity checking */
      break;
    case 'S':
    case 's': /*as no parity*/
      options.c_cflag &= ~PARENB;
      options.c_cflag &= ~CSTOPB;
      break;
    default:
      fprintf(stderr, "Unsupported parity\n");
      return (FAIL);
  }
  /*set stopbits */
  switch (stopbits) {
    case 1:
      options.c_cflag &= ~CSTOPB;
      break;
    case 2:
      options.c_cflag |= CSTOPB;
      break;
    default:
      fprintf(stderr, "Unsupported stop bits\n");
      return (FAIL);
  }
  /* Set input parity options */
  if (parity != 'n')
    options.c_iflag |= INPCK;

  options.c_cc[VTIME] = 0;  // 15 seconds
  options.c_cc[VMIN] = 0;

  tcflush(fd, TCIFLUSH); /* Update the optionsions and do it NOW */

  if (tcsetattr(fd, TCSANOW, &options) != 0) {
    perror("tcsetattr");
    return FAIL;
  }
  return (OK);
}

void LinkTty::start() {
  char dev[16];
  sprintf(dev, "tty%d", link->port);
  fd = open(dev, O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd < 0) {
    perror("open");
    exit(1);
  }
  pthread_t clientHandler;
  pthread_create(&clientHandler, NULL, thread_recvData, this);
}

void LinkTty::send(uint8_t* data, uint16_t dataLen, LinkCfgStruct* dstLink) {
  if (write(fd, data, dataLen) < 0) {
    perror("serial_send");
  }
}

void* LinkTty::thread_recvData(void* x) {
  LinkTty* linkTty = (LinkTty*)x;
  int recvNum;

  while (1) {
    recvNum = read(linkTty->fd, linkTty->recvBuf, BUF_LEN);

    if (recvNum > 0) {
      LinkCfgStruct* srcLink = new LinkCfgStruct;
      srcLink->type = linkType_tty;
      srcLink->port = linkTty->link->port;
      linkTty->recv(linkTty->recvBuf, recvNum, srcLink);
    } else if (recvNum < 0) {
      perror("read from usrt");
      exit(1);
    }
  }
  printf("usrt rcv data stop...\n");
};
