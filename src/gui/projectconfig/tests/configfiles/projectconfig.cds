{
    "connections": [
        {
            "in_id": "{5297a6a3-1be5-4ebe-8c23-9e265cfd4237}",
            "in_index": 0,
            "out_id": "{c38dc1df-2c4b-4ab8-ab47-f1a0053fcc40}",
            "out_index": 0
        },
        {
            "in_id": "{c38dc1df-2c4b-4ab8-ab47-f1a0053fcc40}",
            "in_index": 0,
            "out_id": "{a2ea16be-b823-4a14-84c9-0afa7244c852}",
            "out_index": 0
        },
        {
            "in_id": "{2b5d962a-a8ca-4869-8d62-29a40ec7224b}",
            "in_index": 0,
            "out_id": "{861bfc87-7097-4c72-a3d6-526f129f39e5}",
            "out_index": 0
        },
        {
            "in_id": "{a2ea16be-b823-4a14-84c9-0afa7244c852}",
            "in_index": 0,
            "out_id": "{17a037f2-761d-4bf3-8cdd-45d407797bfb}",
            "out_index": 0
        },
        {
            "in_id": "{1d849bdb-9ccd-4f86-a877-8b6edd8cdef5}",
            "in_index": 0,
            "out_id": "{861bfc87-7097-4c72-a3d6-526f129f39e5}",
            "out_index": 0
        },
        {
            "in_id": "{861bfc87-7097-4c72-a3d6-526f129f39e5}",
            "in_index": 0,
            "out_id": "{1ca2f048-2b27-48ff-a2e4-28f9a1e36c74}",
            "out_index": 0
        }
    ],
    "nodes": [
        {
            "id": "{c38dc1df-2c4b-4ab8-ab47-f1a0053fcc40}",
            "model": {
                "caption": "CanRawFilter #8",
                "name": "CanRawFilter",
                "rxList": [
                    {
                        "id": ".*",
                        "payload": ".*",
                        "policy": true
                    }
                ],
                "txList": [
                    {
                        "id": ".*",
                        "payload": ".*",
                        "policy": true
                    }
                ]
            },
            "position": {
                "x": 461,
                "y": 451
            }
        },
        {
            "id": "{5297a6a3-1be5-4ebe-8c23-9e265cfd4237}",
            "model": {
                "caption": "CanRawLogger #7",
                "directory": ".",
                "name": "CanRawLogger"
            },
            "position": {
                "x": 691,
                "y": 433
            }
        },
        {
            "id": "{861bfc87-7097-4c72-a3d6-526f129f39e5}",
            "model": {
                "backend": "aaa",
                "caption": "CanDevice #1",
                "interface": "bbb",
                "name": "CanDevice"
            },
            "position": {
                "x": 339,
                "y": 190
            }
        },
        {
            "id": "{1d849bdb-9ccd-4f86-a877-8b6edd8cdef5}",
            "model": {
                "caption": "CanRawView #3",
                "name": "CanRawView",
                "scrolling": false,
                "viewColumns": [
                    {
                        "name": "time",
                        "vIdx": 2
                    },
                    {
                        "name": "id",
                        "vIdx": 1
                    },
                    {
                        "name": "dir",
                        "vIdx": 4
                    },
                    {
                        "name": "dlc",
                        "vIdx": 3
                    },
                    {
                        "name": "data",
                        "vIdx": 5
                    }
                ]
            },
            "position": {
                "x": 588,
                "y": 160
            }
        },
        {
            "id": "{1ca2f048-2b27-48ff-a2e4-28f9a1e36c74}",
            "model": {
                "caption": "CanRawSender #2",
                "content": [
                    {
                        "data": "222",
                        "id": "111",
                        "interval": "33",
                        "loop": true,
                        "send": true
                    },
                    {
                        "data": "333",
                        "id": "22",
                        "interval": "33",
                        "loop": false,
                        "send": false
                    }
                ],
                "name": "CanRawSender",
                "senderColumns": [
                    "Id",
                    "Data",
                    "Loop",
                    "Interval",
                    ""
                ],
                "sorting": {
                    "currentIndex": 32722
                }
            },
            "position": {
                "x": 111,
                "y": 170
            }
        },
        {
            "id": "{17a037f2-761d-4bf3-8cdd-45d407797bfb}",
            "model": {
                "caption": "CanRawPlayer #5",
                "file": null,
                "name": "CanRawPlayer",
                "timer tick [ms]": 10
            },
            "position": {
                "x": 23,
                "y": 327
            }
        },
        {
            "id": "{2b5d962a-a8ca-4869-8d62-29a40ec7224b}",
            "model": {
                "bitrate [bps]": "500000",
                "caption": "CanLoad #4",
                "name": "CanLoad",
                "period [ms]": "1000"
            },
            "position": {
                "x": 577,
                "y": 312
            }
        },
        {
            "id": "{a2ea16be-b823-4a14-84c9-0afa7244c852}",
            "model": {
                "backend": null,
                "caption": "CanDevice #6",
                "interface": null,
                "name": "CanDevice"
            },
            "position": {
                "x": 230,
                "y": 346
            }
        }
    ]
}
