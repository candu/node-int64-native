{
  "targets": [
    {
      "target_name": "Int64",
      "include_dirs" : [
        "<!(node -e \"require('nan')\")"
      ],
      "sources": [
        "src/main.cc",
        "src/Int64.cc"
      ]
    }
  ]
}
