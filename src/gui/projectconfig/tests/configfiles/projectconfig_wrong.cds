{
    "connections": [
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
            "id": "{861bfc87-7097-4c72-a3d6-526f129f39e5}",
            "model": {
                "backend": "aaa",
                "caption": "CanDevice #1",
                "interface": "bbb",
                "name": 111
            },
            "position": {
                "x": 339,
                "y": 190
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
                    "currentIndex": 32757
                }
            },
            "position": {
                "x": 111,
                "y": 170
            }
        }
    ]
}
