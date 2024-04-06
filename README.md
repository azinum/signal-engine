# Signal Engine

## Instructions

| Instruction | Description                                                                      | Reads  | Writes |
| ----------- | -------------------------------------------------------------------------------- | ------ | ------ |
| NONE        | Copy input                                                                       | 1-4    | 0      |
| CLOCK       | Increment a counter in a set interval and broadcast to neighbours                | 0      | 0-4    |
| ADD         | Add the sum of two inputs to itself and broadcast to neighbours                  | 2      | 0-4    |
| BUS         | On input broadcast to neighbours                                                 | 1      | 0-4    |
| AND         | Logical AND operation on two inputs and broadcast if the operation returned true | 2      | 0-4    |
| PRINT       | Print input to stdout                                                            | 1-4    | 0 4    |
| INCR        | Increment by the input and broadcast to neighbours                               | 1-4    | 0-4    |
| NOT         | Logical NOT operation on one input and broadcast                                 | 1      | 0-4    |
| COPY        | Copy the input into the output(s)                                                | 1      | 0-4    |
| EQUALS      | Compare equality of two inputs and broadcast the result                          | 2      | 0-4    |
| COPY\_LR    | Copy input from left to right                                                    | 1      | 1      |
| COPY\_RL    | Copy input from right to left                                                    | 1      | 1      |
| COPY\_UD    | Copy input from up to down                                                       | 1      | 1      |
| COPY\_DU    | Copy input from down to up                                                       | 1      | 1      |

## Controls

| Key                      | Description                                                                      |
| ------------------------ | -------------------------------------------------------------------------------- |
| WASD                     | Move camera                                                                      |
| LMB                      | Set the value of a node to 1 and trigger a broadcast to neighbours               |
| Mouse wheel              | Change node type                                                                 |
| Control + Mouse wheel    | Increment or decrement data of a hovered node                                    |
| Control + X              | Cut node                                                                         |
| Control + C              | Copy node                                                                        |
| Control + V              | Paste node                                                                       |
| Control + S              | Save engine state to file                                                        |
| Control + R              | Reload engine state from file                                                    |
| L                        | Open/close logger                                                                |
| Spacebar                 | Play/pause engine                                                                |
| 1                        | Decrease engine tick rate                                                        |
| 2                        | Increase engine tick rate                                                        |
