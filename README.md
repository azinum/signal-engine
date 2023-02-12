# Signal Engine

## Instructions

| Instruction | Description                                                                      | Reads  | Writes |
| ----------- | -------------------------------------------------------------------------------- | ------ | ------ |
| NONE        | Copy input                                                                       | 0-4    | 0      |
| CLOCK       | Increment a counter in a set interval and broadcast to neighbours                | 0      | 0-4    |
| ADD         | Add the sum of two inputs to itself and broadcast to neighbours                  | 2      | 0-4    |
| IO          | Copy input and broadcast to neighbours                                           | 0-4    | 0-4    |
| AND         | Logical AND operation on two inputs and broadcast if the operation returned true | 2      | 0-4    |
| PRINT       | Print input to stdout                                                            | 0-4    | 0 4    |
| INCR        | Increment by the input and broadcast to neighbours                               | 0-4    | 0-4    |
| NOT         | Logical NOT operation on one input and broadcast                                 | 1      | 0-4    |
| COPY        | Copy the input into the output(s)                                                | 1      | 0-4    |
