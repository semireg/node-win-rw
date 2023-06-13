{
  "variables": {
    "module_name%": "win_rw",
    "module_path%": "lib"
  },
  'targets': [
    {
      "target_name": "action_after_build",
      "type": "none",
      "dependencies": [ "<(module_name)" ],
      "copies": [
        {
          "files": [ "<(PRODUCT_DIR)/<(module_name).node" ],
          "destination": "<(module_path)"
        }
      ]
    },
    {
      'target_name': 'win_rw',
      'sources': [
        # is like "ls -1 src/*.cc", but gyp does not support direct patterns on
        # sources
        '<!@(["python3", "tools/getSourceFiles.py", "src", "cc"])'
      ],
      'include_dirs' : [
        "<!(node -e \"require('nan')\")"
      ],
      'cflags_cc+': [
        "-Wno-deprecated-declarations"
      ],
      'conditions': [
        # common exclusions
        ['OS!="linux"', {'sources/': [['exclude', '_linux\\.cc$']]}],
        ['OS!="mac"', {'sources/': [['exclude', '_mac\\.cc|mm?$']]}],
        ['OS!="win"', {
          'sources/': [['exclude', '_win\\.cc$']]}, {
          # else if OS==win, exclude also posix files
          'sources/': [['exclude', '_posix\\.cc$']]
        }]
      ]
    }
  ]
}
