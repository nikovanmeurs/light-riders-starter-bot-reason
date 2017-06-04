/* Welcome to the OCaml Starter for Hackman on Riddles.io
 */
open Td;

open Debug;

let get_time () => Unix.gettimeofday ();

let adjacent = [((-1), 0, "up"), (1, 0, "down"), (0, (-1), "left"), (0, 1, "right")];

/* input processing */
let is_mine gstate i => i == gstate.setup.your_botid;

let uncomment s =>
  try (String.sub s 0 (String.index s '#')) {
  | Not_found => s
  };

let new_cell () => `Empty;

let new_field width height => {
  let v = Array.make_matrix height width 0;
  Array.map (fun x => Array.map (fun y => new_cell ()) x) v
};

let new_player v => {
  let loc = {row: 0, col: 0};
  let prev_loc = {row: 0, col: 0};
  let player = {id: v, loc, prev_loc, dir: `Stop};
  player
};

let clear_gstate gstate => {
  gstate.field = new_field gstate.setup.width gstate.setup.height;
  gstate.round = 0;
  gstate.player = [|new_player 0, new_player 1|]
};

/* tokenizer from rosetta code */
let split_char sep str => {
  let string_index_from i =>
    try (Some (String.index_from str i sep)) {
    | Not_found => None
    };
  let rec aux i acc =>
    switch (string_index_from i) {
    | Some i' =>
      let w = String.sub str i (i' - i);
      aux (succ i') [w, ...acc]
    | None =>
      let w = String.sub str i (String.length str - i);
      List.rev [w, ...acc]
    };
  aux 0 []
};

let which_player =
  fun
  | 0 => `Player0
  | 1 => `Player1
  | n => failwith ("which_player accepts 0 or 1, called with " ^ string_of_int n);

let which_direction (from_row, from_col) (to_row, to_col) =>
  if (from_row == to_row) {
    if (from_col > to_col) {
      `Left
    } else if (from_col < to_col) {
      `Right
    } else {
      `Stop
      /* This means the player has been allowed to stand still */
    }
  } else if (
    from_col == to_col
  ) {
    if (from_row > to_row) {
      `Up
    } else if (from_row < to_row) {
      `Down
    } else {
      `Stop
      /* This means the player has been allowed to stand still */
    }
  } else {
    `Stop
    /* This means the player has teleported somehow */
  };

let update_player_loc gstate player_id row col => {
  let player_loc = gstate.player.(player_id).loc;
  gstate.field.(row).(col) = which_player player_id;
  gstate.field.(player_loc.row).(player_loc.col) = `Wall;
  gstate.player.(player_id).dir =
    which_direction
      (gstate.player.(player_id).loc.row, gstate.player.(player_id).loc.col) (row, col);
  gstate.player.(player_id).prev_loc.row = player_loc.row;
  gstate.player.(player_id).prev_loc.col = player_loc.col;
  gstate.player.(player_id).loc.row = row;
  gstate.player.(player_id).loc.col = col
};

let read_map_string gstate row col s =>
  switch s {
  | "0" => update_player_loc gstate 0 row col
  | "1" => update_player_loc gstate 1 row col
  | "x" => gstate.field.(row).(col) = `Wall
  | _ => gstate.field.(row).(col) = `Empty
  };

let update_game_field (gstate: game_state) data => {
  List.iteri
    (
      fun i (s: string) => {
        let row = i / gstate.setup.width;
        let col = i mod gstate.setup.width;
        read_map_string gstate row col s
      }
    )
    (split_char ',' data);
  ()
};

let action_move bot gstate t2 => {
  gstate.last_timebank = int_of_string t2;
  bot gstate
};

let update_game_round (gstate: game_state) v => {
  gstate.last_update = get_time ();
  gstate.round = int_of_string v;
  ()
};

let update_game (gstate: game_state) k v =>
  switch k {
  | "round" => update_game_round gstate v
  | "field" => update_game_field gstate v
  | _ => ()
  };

let update (gstate: game_state) key t1 t2 =>
  switch key {
  | "game" => update_game gstate t1 t2
  | _ => ()
  };

let four_token (gstate: game_state) key t1 t2 t3 =>
  if (t3 == "" || t2 == "" || t1 == "" || key == "") {
    debug ("four_token fail: " ^ key ^ " " ^ t1 ^ " " ^ t2 ^ " " ^ t3 ^ "\n")
  } else {
    switch key {
    | "update" => update gstate t1 t2 t3
    | _ => ()
    }
  };

let action bot gstate key t1 =>
  switch key {
  | "move" => action_move bot gstate t1
  | _ => ()
  };

let settings gstate key t1 =>
  switch key {
  | "timebank" => gstate.setup.timebank = int_of_string t1
  | "time_per_move" => gstate.setup.time_per_move = int_of_string t1
  | "player_names" => gstate.setup.player_names = split_char ',' t1
  | "your_bot" => gstate.setup.your_bot = t1
  | "your_botid" => gstate.setup.your_botid = int_of_string t1
  | "field_width" =>
    gstate.setup.width = int_of_string t1;
    if (gstate.setup.height > 0) {
      clear_gstate gstate
    }
  | "field_height" =>
    gstate.setup.height = int_of_string t1;
    if (gstate.setup.width > 0) {
      clear_gstate gstate
    }
  | "max_rounds" => gstate.setup.max_rounds = int_of_string t1
  | _ => ()
  };

let three_token bot gstate key t1 t2 =>
  if (t2 == "" || t1 == "" || key == "") {
    debug ("three_token fail: " ^ key ^ " " ^ t1 ^ " " ^ t2 ^ " " ^ "\n")
  } else {
    switch key {
    | "settings" => settings gstate t1 t2
    | "action" => action bot gstate t1 t2
    | _ => ()
    }
  };

let process_line bot gstate line => {
  let tokens = split_char ' ' (uncomment line);
  switch (List.length tokens) {
  | 4 =>
    four_token
      gstate (List.nth tokens 0) (List.nth tokens 1) (List.nth tokens 2) (List.nth tokens 3)
  | 3 => three_token bot gstate (List.nth tokens 0) (List.nth tokens 1) (List.nth tokens 2)
  | _ => debug ("Incorrect bot input: " ^ line ^ "\n")
  }
};

let read_lines bot gstate =>
  while true {
    let line = read_line ();
    process_line bot gstate line
  };

/* End input section */
/* output section */
let issue_order (_, _, s) => {
  Printf.printf "%s\n" s;
  flush stdout
};

let string_of_cell =
  fun
  | `Empty => ". "
  | `Player0 => "0 "
  | `Player1 => "1 "
  | `Wall => "x ";

let debug_field gstate => {
  Array.iter
    (
      fun row => {
        debug "\n";
        Array.iter (fun cell => debug (string_of_cell cell)) row
      }
    )
    gstate.field;
  debug "\n"
};

/* End output section */
/* Utility functions */
let random_from_list lst => {
  let len = List.length lst;
  List.nth lst (Random.int len)
};

let not_reverse gstate player row col =>
  row !== gstate.player.(player).prev_loc.row || col !== gstate.player.(player).prev_loc.col;

let in_bounds gstate row col =>
  row >= 0 && col >= 0 && row < gstate.setup.height && col < gstate.setup.width;

let is_legal gstate player row col =>
  not_reverse gstate player row col && in_bounds gstate row col;

let wrap_loc gstate (row, col) => {
  let out_row =
    if (row < 0) {
      row + gstate.setup.height
    } else if (row >= gstate.setup.height) {
      row - gstate.setup.height
    } else {
      row
    };
  let out_col =
    if (col < 0) {
      row + gstate.setup.width
    } else if (col >= gstate.setup.width) {
      row - gstate.setup.width
    } else {
      col
    };
  (out_row, out_col)
};

let legal_moves (gstate: game_state) player =>
  List.filter
    (fun (row, col, str) => is_legal gstate player row col)
    (
      List.map
        (
          fun (orow, ocol, str) => (
            gstate.player.(player).loc.row + orow,
            gstate.player.(player).loc.col + ocol,
            str
          )
        )
        adjacent
    );

let time_elapsed_this_turn gstate => (get_time () -. gstate.last_update) *. 1000.;

let time_remaining gstate => float_of_int gstate.last_timebank -. time_elapsed_this_turn gstate;

/* End utility */
let init f_get_time => {
  /* get_time := f_get_time (); */
  Random.self_init ();
  let game_info = {
    timebank: 0,
    time_per_move: 0,
    player_names: [],
    your_bot: "",
    your_botid: (-1),
    width: 0,
    height: 0,
    max_rounds: 0
  };
  let game_state = {
    field: [|[||]|],
    round: 0,
    last_update: 0.0,
    last_timebank: 0,
    player: [|new_player 0, new_player 1|],
    setup: game_info
  };
  game_state
};

let feed_bot bot gstate line => process_line bot gstate line;

let run_bot bot => {
  let game_state = init get_time;
  try (read_lines bot game_state) {
  | exc =>
    debug (Printf.sprintf "Exception in turn %d :\n" game_state.round);
    debug (Printexc.to_string exc);
    raise exc
  }
};
