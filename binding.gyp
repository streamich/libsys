{
  "targets": [
    {
      "target_name": "libsys",
      "include_dirs" : [
        "<!(node -e \"require('nan')\")"
      ],
      "sources": [
        "libsys.cc",
        "syscall/syscall.c",
        # "atomics/atomics.c",
        # "async/async.c",
      ],
    }
  ],
}
