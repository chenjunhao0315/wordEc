use egg::*;
use std::process::Command;

define_language! {
    enum VerilogSymbol {
        "+" = Add([Id; 3]),
        "-" = Sub([Id; 3]),
        "*" = Mul([Id; 3]),
        ">>"  = Rs([Id; 3]),
        "<<"  = Ls([Id; 3]),

        "var" = Var([Id; 2]),
        Constant(i32),
        Symbol(Symbol),
    }
}

#[derive(Default)]
struct BitWidth;
impl Analysis<VerilogSymbol> for BitWidth {
    // const, inbitsa, inbitsb, outbits/number
    type Data = Option<(bool, i32, i32, i32)>;

    fn merge(&mut self, _to: &mut Self::Data, _from: Self::Data) -> DidMerge {
        DidMerge(false, false)
    }
    fn make(egraph: &egg::EGraph<VerilogSymbol, BitWidth>, enode: &VerilogSymbol) -> Self::Data {
        let is_constant  = |i: &Id| egraph[*i].data.as_ref().map(|d| d.0);
        // let i_bitwidth_a = |i: &Id| egraph[*i].data.as_ref().map(|d| d.1);
        // let i_bitwidth_b = |i: &Id| egraph[*i].data.as_ref().map(|d| d.2);
        let o_bitwidth_o = |i: &Id| egraph[*i].data.as_ref().map(|d| d.3);
        match enode {
            VerilogSymbol::Constant(num) => { 
                // println!("Const: {}", num); 
                Some((true, 0, 0, *num))
            }
            VerilogSymbol::Symbol(_symbol) => { 
                // println!("Symbol: {}", _symbol); 
                Some((false, 0, 0, 0))
            }
            VerilogSymbol::Var([name, bits]) => { 
                // println!("Var: {} Bits: {}", is_constant(name)?, o_bitwidth_o(bits)?); 
                Some((is_constant(name)?, 0, 0, o_bitwidth_o(bits)?))
            }
            VerilogSymbol::Add([a, b, bits]) => { 
                // println!("Add: {}+{} Bits: {}", *a, *b, *bits); 
                let mut ret = Some((false, o_bitwidth_o(a)?, o_bitwidth_o(b)?, o_bitwidth_o(bits)?));
                if is_constant(a)? && is_constant(b)? {
                    ret = Some((true, 0, 0, o_bitwidth_o(a)? + o_bitwidth_o(b)?))
                }
                ret
            }
            VerilogSymbol::Sub([a, b, bits]) => { 
                // println!("Sub: {}-{} Bits: {}", *a, *b, *bits); 
                let mut ret = Some((false, o_bitwidth_o(a)?, o_bitwidth_o(b)?, o_bitwidth_o(bits)?));
                if is_constant(a)? && is_constant(b)? {
                    ret = Some((true, 0, 0, o_bitwidth_o(a)? - o_bitwidth_o(b)?))
                }
                ret
            }
            VerilogSymbol::Mul([a, b, bits]) => { 
                // println!("Mul: {}+{} Bits: {}", *a, *b, *bits); 
                let mut ret = Some((false, o_bitwidth_o(a)?, o_bitwidth_o(b)?, o_bitwidth_o(bits)?));
                if is_constant(a)? && is_constant(b)? {
                    ret = Some((true, 0, 0, o_bitwidth_o(a)? * o_bitwidth_o(b)?))
                }
                ret
            }
            VerilogSymbol::Ls([a, b, bits]) => { 
                // println!("Ls: {}+{} Bits: {}", *a, *b, *bits); 
                let mut ret = Some((false, o_bitwidth_o(a)?, o_bitwidth_o(b)?, o_bitwidth_o(bits)?));
                if is_constant(a)? && is_constant(b)? {
                    ret = Some((true, 0, 0, o_bitwidth_o(a)? << o_bitwidth_o(b)?))
                }
                ret
            }
            VerilogSymbol::Rs([a, b, bits]) => { 
                // println!("Rs: {}+{} Bits: {}", *a, *b, *bits); 
                let mut ret = Some((false, o_bitwidth_o(a)?, o_bitwidth_o(b)?, o_bitwidth_o(bits)?));
                if is_constant(a)? && is_constant(b)? {
                    ret = Some((true, 0, 0, o_bitwidth_o(a)? >> o_bitwidth_o(b)?))
                }
                ret
            }
            // _ => None,
        }
    }
}

type EGraph = egg::EGraph<VerilogSymbol, BitWidth>;

struct PolyCostFn;
impl CostFunction<VerilogSymbol> for PolyCostFn {
    type Cost = f64;
    fn cost<C>(&mut self, enode: &VerilogSymbol, mut costs: C) -> Self::Cost
    where
        C: FnMut(Id) -> Self::Cost
    {
        let temp:&str = &mut enode.to_string();
        let op_cost = match temp {
            "var" => 0.0,
            "+" => 1.0,
            "-" => 1.0,
            "*" => 1.0,
            ">>" => 100.0, 
            "<<" => 100.0,

            _ => 0.0
        };
        enode.fold(op_cost, |sum, id| sum + costs(id))
    }
}

fn is_zero(var: &str) -> impl Fn(&mut EGraph, Id, &Subst) -> bool {
    let var = var.parse().unwrap();
    move |egraph, _, subst| {
        if let Some(_n) = &egraph[subst[var]].data {
            let extractor = Extractor::new(&egraph, PolyCostFn);
            let (_cost, expr) = extractor.find_best(subst[var]);
            let status = Command::new("./zero_eq_cl").arg(expr.to_string()).arg("0").status().expect("failed to execute process");

            let mut decision = false;
            match status.code() {
                Some(code) => {if code == 1 { decision = true; }}
                None => { false; }
            }

            decision
        } else {
            false
        }
    }
}

fn is_not_zero(var: &str) -> impl Fn(&mut EGraph, Id, &Subst) -> bool {
    let var = var.parse().unwrap();
    move |egraph, _, subst| {
        if let Some(_n) = &egraph[subst[var]].data {
            let extractor = Extractor::new(&egraph, PolyCostFn);
            let (_cost, expr) = extractor.find_best(subst[var]);
            let status = Command::new("./zero_eq_cl").arg(expr.to_string()).arg("0").status().expect("failed to execute process");

            let mut decision = false;
            match status.code() {
                Some(code) => {if code == 1 { decision = true; }}
                None => { false; }
            }

            !decision
        } else {
            true
        }
    }
}

fn is_one(var: &str) -> impl Fn(&mut EGraph, Id, &Subst) -> bool {
    let var = var.parse().unwrap();
    move |egraph, _, subst| {
        if let Some(_n) = &egraph[subst[var]].data {
            let extractor = Extractor::new(&egraph, PolyCostFn);
            let (_cost, expr) = extractor.find_best(subst[var]);
            let status = Command::new("./zero_eq_cl").arg(expr.to_string()).arg("1").status().expect("failed to execute process");

            let mut decision = false;
            match status.code() {
                Some(code) => {if code == 1 { decision = true; }}
                None => { false; }
            }

            decision
        } else {
            false
        }
    }
}

fn make_rules() -> Vec<Rewrite<VerilogSymbol, BitWidth>> {
    let rules = vec![
        rewrite!("comm-mul";  "(* ?a ?b ?bit)" => "(* ?b ?a ?bit)" if is_not_zero("?b")),
        rewrite!("assoc-mul"; "(* ?a (* ?b ?c ?biti) ?bito)" => "(* ?b (* ?c ?a ?bito) ?bito)"),
        rewrite!("comm-add";  "(+ ?a ?b ?bit)" => "(+ ?b ?a ?bit)" if is_not_zero("?b")),
        rewrite!("assoc-add"; "(+ ?a (+ ?b ?c ?biti) ?bito)" => "(+ ?b (+ ?c ?a ?bito) ?bito)" if is_not_zero("?a")),
        rewrite!("distribute-mul-over-add"; "(* ?a (+ ?b ?c ?biti) ?bito)" => "(+ (* ?a ?b ?bito) (* ?a ?c ?bito) ?bito)" if is_not_zero("?a")),
        rewrite!("sum-same"; "(+ ?a ?a ?bit)" => "(* 2 ?a ?bit)" if is_not_zero("?a")),
        rewrite!("mul-sum-same"; "(+ (* ?a ?b ?bit) ?b ?bito)" => "(* (+ ?a 1 ?bito) ?b ?bito)" if is_not_zero("?b")),
        rewrite!("add-zero"; "(+ ?a ?b ?bit)" => "?a" if is_zero("?b")),
        rewrite!("mul-zero"; "(* ?a ?b ?bit)" => "0" if is_zero("?b")),
        rewrite!("mul-by-one"; "(* ?a ?b ?bit)" => "?a" if is_one("?b")),
        rewrite!("mul-by-two";  "(* 2 ?a ?bit)" => "(<< ?a 1 ?bit)" if is_not_zero("?a")),

        rewrite!("sub-canon"; "(- ?a ?b ?bit)" => "(+ ?a (* -1 ?b ?bit) ?bit)"),
        rewrite!("canon-sub"; "(+ ?a (* -1 ?b ?bit) ?bito)"   => "(- ?a ?b ?bito)"),
        rewrite!("cancel-sub"; "(- ?a ?a ?bit)" => "0"),

        rewrite!("merge-left-shift"; "(<< (<< ?a ?b ?bit) ?c ?bito)" => "(<< ?a (+ ?b ?c ?bito) ?bito)"),
        rewrite!("unmerge-left-shift"; "(<< ?a (+ ?b ?c ?bit) ?bito)" => "(<< (<< ?a ?b ?bito) ?c ?bito)"),
        rewrite!("merge-right-shift"; "(>> (>> ?a ?b ?bit) ?c ?bito)" => "(>> ?a (+ ?b ?c ?bito) ?bito)"),
        rewrite!("unmerge-right-shift"; "(>> ?a (+ ?b ?c ?bit) ?bito)" => "(>> (>> ?a ?b ?bito) ?c ?bito)"),

        rewrite!("left-shift-add"; "(<< (+ ?a ?b ?bit) ?c ?bito)" => "(+ (<< ?a ?c ?bito) (<< ?b ?c ?bito) ?bito)"),
        rewrite!("add-right-shift"; "(+ ?a (>> ?b ?c ?bit) ?bito)" => "(>> (+ (<< ?a ?c ?bito) ?b ?bito) ?c ?bito)"),
        rewrite!("left-shift-mul"; "(<< (* ?a ?b ?bit) ?c ?bito)" => "(* ?a (<< ?b ?c ?bito) ?bito)"),
        rewrite!("mul-left-shift"; "(* ?a (<< ?b ?c ?bit) ?bito)" => "(<< (* ?a ?b ?bito) ?c ?bito)")
    ];
    rules
}

fn enode_zero_eq_merge(egraph: &mut EGraph) {
    let mut merge = Vec::new();
    for eclass1 in egraph.classes().map(|e| e.id) {
        let extractor1 = Extractor::new(&egraph, PolyCostFn);
        let (_cost1, expr1) = extractor1.find_best(eclass1);
        for eclass2 in egraph.classes().map(|e| e.id) {
            if eclass1 == eclass2 {
                continue;
            }
            let extractor2 = Extractor::new(&egraph, PolyCostFn);
            let (_cost1, expr2) = extractor2.find_best(eclass2);

            let status = Command::new("./zero_eq_cl").arg(expr1.to_string()).arg(expr2.to_string()).status().expect("failed to execute process");

            match status.code() {
                Some(code) => {if code == 1 { merge.push((eclass1, eclass2)); }}
                None => { false; }
            }
        }
    }

    for merge_class in merge {
        egraph.union_trusted(merge_class.0, merge_class.1, "zero_equal");
    }
}

fn main() {
    // zero_eq
    let expr_spec : RecExpr<VerilogSymbol> = "(+ (- (+ (+ (+ (* 16384 (* (* (var A 12) (var A 12) 16) (* (var A 12) (var A 12) 16) 16) 16) (* 16384 (* (* (var B 8) (var B 8) 16) (* (var B 8) (var B 8) 16) 16) 16) 16) (- (* 64767 (* (var A 12) (var A 12) 16) 16) (* 64767 (* (var B 8) (var B 8) 16) 16) 16) 16) (var A 12) 16) (var B 8) 16) (* (* (* 57344 (var A 12) 16) (var B 8) 16) (- (var A 12) (var B 8) 16) 16) 16)".parse().unwrap();
    let expr_impl : RecExpr<VerilogSymbol> = "(+ (+ (+ (+ (+ (+ (* 24576 (* (var B 8) (* (var A 12) (var A 12) 16) 16) 16) (* 15615 (* (var A 12) (var A 12) 16) 16) 16) (* 8192 (* (var A 12) (* (var B 8) (var B 8) 16) 16) 16) 16) (* 32768 (* (var A 12) (var B 8) 16) 16) 16) (var A 12) 16) (* 17153 (* (var B 8) (var B 8) 16) 16) 16) (* 65535 (var B 8) 16) 16)".parse().unwrap();

    // // zero_eq
    // let expr_spec : RecExpr<VerilogSymbol> = "(* (<< (var A 16) (var M 4) 31) (<< (var B 16) (var N 4) 31) 63)".parse().unwrap();
    // let expr_impl : RecExpr<VerilogSymbol> = "(<< (* (var A 16) (var B 16) 32) (+ (var M 4) (var N 4) 5) 63)".parse().unwrap();

    // // zero_eq
    // let expr_spec : RecExpr<VerilogSymbol> = "(* 4 (* (var x 3) (var x 3) 3) (3))".parse().unwrap();
    // let expr_impl : RecExpr<VerilogSymbol> = "(* 4 (var x 3) 4)".parse().unwrap();

    // // zero_eq
    // let expr_spec : RecExpr<VerilogSymbol> = "(* 8 (var x 3) (3))".parse().unwrap();
    // let expr_impl : RecExpr<VerilogSymbol> = "(0)".parse().unwrap();

    // // assoc-mul
    // let expr_spec : RecExpr<VerilogSymbol> = "(+ 4 (+ (var y 3) (+ (var x 3) (var x 3) (3)) (3)) (3))".parse().unwrap();
    // let expr_impl : RecExpr<VerilogSymbol> = "(+ 4 (+ (var x 3) (+ (var y 3) (var x 3) (3)) (3)) (3))".parse().unwrap();

    // // distribute-mul-over-add
    // let expr_spec : RecExpr<VerilogSymbol> = "(* (var x 3) (+ (var y 3) (var z 3) (3)) (3))".parse().unwrap();
    // let expr_impl : RecExpr<VerilogSymbol> = "(+ (* (var x 3) (var y 3) (3)) (* (var x 3) (var z 3) (3)) (3))".parse().unwrap();

    // // mul-sum-same
    // let expr_spec : RecExpr<VerilogSymbol> = "(+ (* (var x 3) (var y 3) (3)) (var y 3) (3))".parse().unwrap();
    // let expr_impl : RecExpr<VerilogSymbol> = "(* (+ (var x 3) (1) (3)) (var y 3) (3))".parse().unwrap();

    // let expr_spec : RecExpr<VerilogSymbol> = "(* 4 (* (var y 3) (* (var x 3) (var x 3) (3)) (3)) (3))".parse().unwrap();
    // let expr_impl : RecExpr<VerilogSymbol> = "(* 4 (* (var x 3) (* (var y 3) (var x 3) (3)) (3)) (3))".parse().unwrap();

    // let expr_spec : RecExpr<VerilogSymbol> = "(* (var x 3) (var y 3) (3))".parse().unwrap();
    // let expr_impl : RecExpr<VerilogSymbol> = "(* (var y 3) (var x 3) (3))".parse().unwrap();

    // // left-shift-add
    // let expr_spec : RecExpr<VerilogSymbol> = "(<< (+ (var a 3) (var b 3) 3) (var c 3) (3))".parse().unwrap();
    // let expr_impl : RecExpr<VerilogSymbol> = "(+ (<< (var a 3) (var c 3) 4) (<< (var b 3) (var c 3) 4) (3))".parse().unwrap();

    // // add-right-shift
    // let expr_spec : RecExpr<VerilogSymbol> = "(<< (+ (var a 3) (var b 3) 3) (var c 3) 3)".parse().unwrap();
    // let expr_impl : RecExpr<VerilogSymbol> = "(>> (+ (<< (var a 3) (var c 3) 3) (var b 3) 3) (var c 3) 3)".parse().unwrap();

    // // left-shift-mul
    // let expr_spec : RecExpr<VerilogSymbol> = "(<< (* (var a 3) (var b 3) 3) (var c 3) 3)".parse().unwrap();
    // let expr_impl : RecExpr<VerilogSymbol> = "(* (<< (var a 3) (var c 3) 3) (var c 3) 3)".parse().unwrap();

    // // merge-left-shift
    // let expr_spec : RecExpr<VerilogSymbol> = "(<< (<< (var a 3) (var b 3) 3) (var c 3) 3)".parse().unwrap();
    // let expr_impl : RecExpr<VerilogSymbol> = "(<< (var a 3) (+ (var b 3) (var c 3) 3) 3)".parse().unwrap();

    // // merge-right-shift
    // let expr_spec : RecExpr<VerilogSymbol> = "(>> (>> (var a 3) (var b 3) 3) (var c 3) 3)".parse().unwrap();
    // let expr_impl : RecExpr<VerilogSymbol> = "(>> (var a 3) (+ (var b 3) (var c 3) 3) 3)".parse().unwrap();

    let mut runner = Runner::<VerilogSymbol, BitWidth, ()>::default();
    runner = runner.with_expr(&expr_spec);
    runner = runner.with_expr(&expr_impl);

    let mut equiv = false;
    let max_iter = 1;
    for _i in 0..max_iter {
        // rewrite
        runner = runner.with_iter_limit(1).run(&make_rules());
        // rebuild
        enode_zero_eq_merge(&mut runner.egraph);
        runner.egraph.rebuild();

        let ids = runner.egraph.equivs(&expr_spec, &expr_impl);
        if ids.len() > 0 {
            equiv = true;
            break;
        }
        runner.stop_reason = None;
    }
    
    println!("expr_spec: {}", expr_spec.to_string());
    println!("expr_impl: {}", expr_impl.to_string());
    if equiv {
        println!("expr_spec and expr_impl are equivalence");
    } else {
        println!("expr_spec and expr_impl are \"not\" equivalence");
    }

    runner.egraph.dot().to_png("test.png").unwrap();
}