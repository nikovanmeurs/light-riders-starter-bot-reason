open Td;

let main gstate => {
  let moves = Game.legal_moves gstate gstate.setup.your_botid;
  let move = Game.random_from_list moves;
  Game.issue_order move
};

Game.run_bot main;
