{
   "modul":[
      {
         "Id":1,
         "properties":[
            {
               "Id":1,
               "Type":2,
               "Flags":0,
               "Name":"Name of module",
               "Default":"CanDevice"
            },
            {
               "Id":1002,
               "Type":3,
               "Flags":1,
               "Name":"interface",
               "Default":"1",
               "LimitId":1002
            },
            {
               "Id":1003,
               "Type":3,
               "Flags":1,
               "Name":"backend",
               "Default":"0",
               "LimitId":1003
            }
         ],
         "limits":[
            {
               "Id":1002,
               "Type":3,
               "Enum":[
                  {
                     "Value":"0",
                     "Name":"can0"
                  },
                  {
                     "Value":"1",
                     "Name":"can1"
                  }
               ]
            },
            {
               "Id":1003,
               "Type":3,
               "Enum":[
                  {
                     "Value":"0",
                     "Name":"socketcan"
                  }
               ]
            }
         ]
      },
      {
         "Id":2,
         "properties":[
            {
               "Id":1,
               "Type":2,
               "Flags":0,
               "Name":"Name of module",
               "Default":"CanRawSender"
            },
            {
               "Id":10,
               "Type":2,
               "Flags":0,
               "Name":"Column 1 name",
               "Default":"Id"
            },
            {
               "Id":11,
               "Type":3,
               "Flags":0,
               "Name":"Column 1 visual index",
               "Default":"1",
               "MinValue":"1",
               "MaxValue":"5"
            },
            {
               "Id":12,
               "Type":3,
               "Flags":0,
               "Name":"Column 1 width",
               "Default":"80",
               "MinValue":"0",
               "MaxValue":"300"
            },
            {
               "Id":20,
               "Type":2,
               "Flags":0,
               "Name":"Column 2 name",
               "Default":"Data"
            },
            {
               "Id":21,
               "Type":3,
               "Flags":0,
               "Name":"Column 2 visual index",
               "Default":"2",
               "MinValue":"1",
               "MaxValue":"5"
            },
            {
               "Id":22,
               "Type":3,
               "Flags":0,
               "Name":"Column 2 width",
               "Default":"180",
               "MinValue":"0",
               "MaxValue":"300"
            },
            {
               "Id":30,
               "Type":2,
               "Flags":0,
               "Name":"Column 3 name",
               "Default":"Loop"
            },
            {
               "Id":31,
               "Type":3,
               "Flags":0,
               "Name":"Column 3 visual index",
               "Default":"3",
               "MinValue":"1",
               "MaxValue":"5"
            },
            {
               "Id":32,
               "Type":3,
               "Flags":0,
               "Name":"Column 3 width",
               "Default":"70",
               "MinValue":"0",
               "MaxValue":"300"
            },
            {
               "Id":40,
               "Type":2,
               "Flags":0,
               "Name":"Column 4 name",
               "Default":"Interval"
            },
            {
               "Id":41,
               "Type":3,
               "Flags":0,
               "Name":"Column 4 visual index",
               "Default":"4",
               "MinValue":"1",
               "MaxValue":"5"
            },
            {
               "Id":42,
               "Type":3,
               "Flags":0,
               "Name":"Column 4 width",
               "Default":"120",
               "MinValue":"0",
               "MaxValue":"300"
            },
            {
               "Id":50,
               "Type":2,
               "Flags":0,
               "Name":"Column 5 name",
               "Default":" "
            },
            {
               "Id":51,
               "Type":3,
               "Flags":0,
               "Name":"Column 5 visual index",
               "Default":"5",
               "MinValue":"1",
               "MaxValue":"5"
            },
            {
               "Id":52,
               "Type":3,
               "Flags":0,
               "Name":"Column 5 width",
               "Default":"80",
               "MinValue":"0",
               "MaxValue":"300"
            }
         ]
      },
      {
         "Id":3,
         "properties":[
            {
               "Id":1,
               "Type":2,
               "Flags":0,
               "Name":"Name of module",
               "Default":"CanRawViewer"
            },
            {
               "Id":10,
               "Type":2,
               "Flags":0,
               "Name":"Column 1 name",
               "Default":"Time"
            },
            {
               "Id":11,
               "Type":3,
               "Flags":0,
               "Name":"Column 1 visual index",
               "Default":"1",
               "MinValue":"1",
               "MaxValue":"5"
            },
            {
               "Id":12,
               "Type":3,
               "Flags":0,
               "Name":"Column 1 width",
               "Default":"80",
               "MinValue":"0",
               "MaxValue":"300"
            },
            {
               "Id":20,
               "Type":2,
               "Flags":0,
               "Name":"Column 2 name",
               "Default":"Id"
            },
            {
               "Id":21,
               "Type":3,
               "Flags":0,
               "Name":"Column 2 visual index",
               "Default":"2",
               "MinValue":"1",
               "MaxValue":"5"
            },
            {
               "Id":22,
               "Type":3,
               "Flags":0,
               "Name":"Column 2 width",
               "Default":"80",
               "MinValue":"0",
               "MaxValue":"300"
            },
            {
               "Id":30,
               "Type":2,
               "Flags":0,
               "Name":"Column 3 name",
               "Default":"Dir"
            },
            {
               "Id":31,
               "Type":3,
               "Flags":0,
               "Name":"Column 3 visual index",
               "Default":"3",
               "MinValue":"1",
               "MaxValue":"5"
            },
            {
               "Id":32,
               "Type":3,
               "Flags":0,
               "Name":"Column 3 width",
               "Default":"70",
               "MinValue":"0",
               "MaxValue":"300"
            },
            {
               "Id":40,
               "Type":2,
               "Flags":0,
               "Name":"Column 4 name",
               "Default":"Dlc"
            },
            {
               "Id":41,
               "Type":3,
               "Flags":0,
               "Name":"Column 4 visual index",
               "Default":"4",
               "MinValue":"1",
               "MaxValue":"5"
            },
            {
               "Id":42,
               "Type":3,
               "Flags":0,
               "Name":"Column 4 width",
               "Default":"70",
               "MinValue":"0",
               "MaxValue":"300"
            },
            {
               "Id":50,
               "Type":2,
               "Flags":0,
               "Name":"Column 5 name",
               "Default":"Data"
            },
            {
               "Id":51,
               "Type":3,
               "Flags":0,
               "Name":"Column 5 visual index",
               "Default":"5",
               "MinValue":"1",
               "MaxValue":"5"
            },
            {
               "Id":52,
               "Type":3,
               "Flags":0,
               "Name":"Column 5 width",
               "Default":"250",
               "MinValue":"0",
               "MaxValue":"400"
            }
         ]
      }
   ]
}
