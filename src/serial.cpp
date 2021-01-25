int readline(int readChar, char *buffer, int len) {
  static int pos = 0;
  int rpos;

  if (readChar > 0) {
    switch (readChar) {
      case '\r': // ignore CR
        break;
      case '\n': // return on new line
        rpos = pos;
        pos = 0; // Reset position index ready for next time
        return rpos;
      default:
        if (pos < len - 1) {
          buffer[pos++] = readChar;
          buffer[pos] = 0;
        }
    }
  }
  return 0;
}
