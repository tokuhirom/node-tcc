{
    'targets': [
        {
            'target_name': 'tcc',
            'sources': [
                './addon.cc',
            ],
            'libraries': [
                '-ltcc'
            ],
            'cflags!': [ '-fno-exceptions' ],
            'cflags_cc!': [ '-fno-exceptions' ],
            'conditions': [
                ['OS=="mac"', {
                    'xcode_settings': {
                        'OTHER_LDFLAGS': [
                            '-ltcc'
                        ]
                    },
                }],
            ]
        },
    ]
}
