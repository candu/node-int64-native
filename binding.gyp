{
  "targets": [
    {
      "target_name": "Int64",
      "sources": [
        "src/int64_main.cc",
        "src/Int64.cc"
      ],
      "include_dirs" : [
        "<!(node -e \"require('nan')\")"
      ],
    },
    {
      "target_name": "UInt64",
      "sources": [
        "src/uint64_main.cc",
        "src/UInt64.cc"
      ],
      "include_dirs" : [
        "<!(node -e \"require('nan')\")"
      ],
    }
  ]
}
