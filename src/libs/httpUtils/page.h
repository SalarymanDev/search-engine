#pragma once

class Page {
  public:
  Page(const char* Url);
  ~Page();
  char *url, *host, *path, *port, *service;

  private:
  char* pathBuffer;
};
