String formatBytes(size_t bytes) {
  if (bytes < 1024) {
    return String(bytes) + "B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + "KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  }
}

// sends name:value
void wsSendValue(const char *name, int value) {
  ws.printfAll("%s:%u", name, value);
}

uint16_t splitString(String in, String separator, String *out) {
  if (in == NULL || in.equals("")) return 0;
  int16_t indOf = in.indexOf(separator);
  if (indOf == -1) {
    out[0] = in;
    return 1;
  }
  uint8_t separatorLength = separator.length();
  uint16_t count = 1;
  String end = in.substring(indOf+separatorLength);
  out[0] = in.substring(0, indOf);
  while ((indOf = end.indexOf(separator)) != -1) {
    out[count] = end.substring(0, indOf);
    count ++;
    end = in.substring(indOf+separatorLength);
  }
  out[count] = end;
  return count+1;
}

uint16_t countSeparatorsInString(String in, String separator) {
  if (in == NULL || in.equals("")) return 0;
  int16_t indOf = in.indexOf(separator);
  if (indOf == -1) return 1;
  uint8_t separatorLength = separator.length();
  uint16_t count = 1;
  String end = in.substring(indOf+separatorLength);
  while ((indOf = end.indexOf(separator)) != -1) {
    count ++;
    end = in.substring(indOf+separatorLength);
  }
  return count+1;
}

