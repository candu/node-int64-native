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
    }
  ]
}
