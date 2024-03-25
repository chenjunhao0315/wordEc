import argparse
import sympy as sym

def parse_arguments():
    parser = argparse.ArgumentParser()
    parser.add_argument('--file', default = "./benchmark/case04_cir1.poly")
    parser.add_argument('--output', default = "./rewrite_smt.constraint")
    parser.add_argument('--output_word', default = "./rewrite_smt.word")
    parser.add_argument('--enable_signed', default=True)

    return parser.parse_args()

def declare(vars, type, output=None, silent=False):
    if not isinstance(vars, list):
        vars = [vars]
    for var in vars:
        if not silent:
            print("(declare-const {} {})".format(var, type))
        if output:
            output.write("(declare-const {} {})\n".format(var, type))

def assert_eq(expr, value, output=None, silent=False):
    if not silent:
        print("(assert (= {} {}))".format(value, expr))
    if output:
        output.write("(assert (= {} {}))\n".format(value, expr))

def assert_range(expr, lower, upper, output=None, silent=False):
    if not silent:
        print("(assert (and (>= {} {}) (< {} {})))".format(expr, lower, expr, upper))
    if output:
        output.write("(assert (and (>= {} {}) (< {} {})))\n".format(expr, lower, expr, upper))

def assert_imply(a, b, output=None, silent=False):
    if not silent:
        print("(assert (=> {} {}))".format(a, b))
    if output:
        output.write("(assert (=> {} {}))\n".format(a, b))

def createWord(name, inbit, outbit, enable_signed, output=None, silent=False):
    # Declare variables
    coeff_bit = [name + "_coeff_" + str(i) for i in range(inbit)]
    coeff = name + "_coeff"
    declare(coeff_bit, "Int", output, silent)
    declare(coeff, "Int", output, silent)

    neg = [name + "_neg_" + str(i) for i in range(inbit)]
    declare(neg, "Bool", output, silent)

    bit_coeff_bit = []
    for i in range(inbit):
        bit_coeff_bit.append([name + "_bit_" + str(i) + "_coeff_" + str(j) for j in range(outbit)])

    for bit_coeff in bit_coeff_bit:
        declare(bit_coeff, "Bool", output, silent)

    bit_coeff = [name + "_bit_" + str(i) for i in range(outbit)]
    declare(bit_coeff, "Bool", output, silent)

    signed = name + "_signed"
    declare(signed, "Bool", output, silent)

    # Generate Constraint
    for i in range(inbit):
        beta = name + "_beta_" + str(i)
        beta_expr = ""
        for j in range(outbit - enable_signed):
            if j == 0:
                beta_expr += bit_coeff_bit[i][j] + " "
            else:
                beta_expr += "(* {} {}) ".format(2**j, bit_coeff_bit[i][j])

        if (enable_signed):
            beta_expr += "(* (ite {} -{} {}) {})".format(signed, 2**(outbit-1), 2**(outbit-1), bit_coeff_bit[i][outbit-1])
        beta_expr = "(+ " + beta_expr + ")"
        declare(beta, "Int", output, silent)
        assert_eq(beta_expr, beta, output, silent)
        # if (enable_signed):
        #     assert_range(beta, -2**(outbit-1), 2**outbit, output)
        # else:
        #     assert_range(beta, 0, 2**outbit, output)

        constraint = "(ite {} (- {}) {})".format(neg[i], beta, beta)
        assert_eq(constraint, coeff_bit[i], output, silent)

    constraint = ""
    for i in range(inbit):
        constraint += "(* {} {}_beta_{}) ".format(neg[i], name, str(i))

    sub_expr = ""
    for j in range(outbit - enable_signed):
        if j == 0:
            sub_expr += bit_coeff[j] + " "
        else:
            sub_expr += "(* {} {}) ".format(2**j, bit_coeff[j])

    if (enable_signed):
        sub_expr += "(* (ite {} -{} {}) {})".format(signed, 2**(outbit-1), 2**(outbit-1), bit_coeff[outbit-1])

    constraint = "(+ " + constraint + sub_expr + ")"
    assert_eq(constraint, coeff, output, silent)

    for i in range(outbit):
        constraint = "(assert ((_ at-most 1) " + bit_coeff[i]
        for j in range(inbit):
            constraint += " " + bit_coeff_bit[j][i]
        constraint += "))"
        if not silent:
            print(constraint)
        if output:
            output.write(constraint + "\n")

    # Polynomial Generation
    coeff_bit_sym = [sym.Symbol(var) for var in coeff_bit]
    var_bit_sym = [sym.Symbol(name + "_" + str(i)) for i in range(inbit)]
    poly = sym.Symbol(name + "_coeff")
    for i in range(inbit):
        poly = poly + coeff_bit_sym[i] * var_bit_sym[i]

    return poly

def assert_coeff(expr, coeff, mod, output=None, silent=False):
    sub_expr = ""
    single = True
    for mono in expr.args:
        sub_sub_expr = ""
        for m in mono.args:
            sub_sub_expr += str(m) + " "
        if len(sub_sub_expr) == 0:
            single &= True
            sub_sub_expr = str(mono)
        else:
            single &= False
            sub_sub_expr = "(* " + sub_sub_expr + ")"
        sub_expr = sub_expr + " " + sub_sub_expr
    if single:
        sub_expr = "(*" + sub_expr + ")"
    else:
        sub_expr = "(+" + sub_expr + ")"

    if not silent:
        print("(assert (= {} (mod {} {})))".format(coeff, sub_expr, mod))
    if output:
        output.write("(assert (= {} (mod {} {})))\n".format(coeff, sub_expr, mod))

# a / b
def poly_div(a, b, ignore):
    monomials = a.args
    expr = sym.Integer(0)
    # print(a, b)
    if (a.is_Mul):
        if (a.has(b)):
            quo = a / b
            expr += quo.expand()
    else:
        for mono in monomials:
            # t = sym.div(mono, b)
            # print(t)
            if (mono.has(b)):
                # print("Has")
                quo = mono / b
                keep = True
                for q in quo.args:
                    if q in ignore:
                        keep = False
                        break
                if keep:
                    expr += quo.expand()
    
    return expr.expand()

def comment(describe, output = None, silent=False):
    if not isinstance(describe, str):
        describe = str(describe)
    if not silent:
        print("; {}".format(describe))
    if output:
        output.write("{}\n".format(describe))

def rewrite_smt(args, silent=False):
    print("Generate SMT ... ", end='')
    with open(args.file, "r") as f:
        output = open(args.output, "w")
        output_word = open(args.output_word, "w")

        outbit = int(f.readline())
        var_num = int(f.readline())
        if output_word:
            comment(var_num, output_word, silent)

        # Read variables
        vars_vec = []
        vars = dict()
        for i in range(var_num):
            var = f.readline().rstrip().split(' ')
            vars[var[0]] = int(var[1])
            vars_vec.append(var[0])
            comment("{} {}".format(var[0], var[1]), output_word, silent)

        # Create Variables
        polys = dict()
        for var in vars.keys():
            bit = vars[var]
            polys[var] = createWord(var, bit, outbit, int(args.enable_signed), output, silent)

        # Read Support Information
        for i in range(var_num):
            uncertain = 0
            order = [[vars_vec[i] + "_" + str(j) for j in range(vars[vars_vec[i]])]]
            used = set()
            for j in range(vars[vars_vec[i]]):
                sup = f.readline().rstrip().split(" ")
                cur = sup.pop(0)
                sup.pop(0)
                # print(cur, sup)
                if len(sup) == 0:
                    uncertain += 1
                    continue
                index = -1
                for k in range(len(order)):
                    if cur in order[k]:
                        index = k
                        break
                popout = []
                used.add(cur)
                for k in range(len(sup)):
                    if sup[k].split("_")[0] == vars_vec[i]:
                        used.add(sup[k]) 
                    for l in range(0, index+1):
                        if sup[k] in order[l]:
                            order[l].remove(sup[k])
                            popout.append(sup[k])

                if index == len(order)-1:
                    order.append(popout)
                else:
                    order[index+1] += popout

            popout = []
            for j in range(len(order)):
                for k in range(len(order[j])):
                    if order[j][k] not in used:
                        popout.append(order[j][k])
                for pop in popout:
                    order[j].remove(pop)

            bucket_start = 0
            bucket_end = len(order[0])
            # print(order)
            for j in range(len(order)):
                # print(bucket_start, bucket_end, min(bucket_end + uncertain, vars[vars_vec[i]]))
                valid = range(j, min(bucket_end + uncertain, min(outbit, vars[vars_vec[i]])))
                for var in order[j]:
                    for l in range(min(outbit, vars[vars_vec[i]])):
                        if l in valid:
                            continue
                        if not silent:
                            print("(assert (not {}_bit_{}_coeff_{}))".format(vars_vec[i], var.split("_")[1], l))
                        if output:
                            output.write("(assert (not {}_bit_{}_coeff_{}))\n".format(vars_vec[i], var.split("_")[1], l))
                        
                bucket_start = bucket_end
                bucket_end += len(order[j+1]) if j < len(order)-1 else 0

            # print(order, uncertain)
            for j in range(len(order)):
                for k in range(0, j):
                    # bucket2 < bucket1
                    bucket1 = order[j]
                    bucket2 = order[k]
                    for b1 in bucket1:
                        var1 = vars_vec[i] + "_bit_" + b1.split("_")[1] + "_coeff_"
                        for b2 in bucket2:
                            var2 = vars_vec[i] + "_bit_" + b2.split("_")[1] + "_coeff_"
                            
                            for m in range(min(outbit, vars[vars_vec[i]])):
                                for n in range(m, min(outbit, vars[vars_vec[i]])):
                                    assert_imply(var1 + str(m), "(not " + var2 + str(n) + ")", output, silent)

        # Get Terms
        terms = f.readlines()
        terms = [term.rstrip() for term in terms]
        def countMul(term):
            return term.count("*")
        terms.sort(key=countMul, reverse=True)

        # Mono Variable
        term_set = set()
        for var in vars_vec:
            term_set.add(frozenset([var]))

        # Product Term
        # TODO: Power of variable
        for term in terms:
            t = term.split('*')
            t.pop(0)
            t = [x.split("_")[0] for x in t]
            term_set.add(frozenset(t))

        # Form Assumption
        mono_num = 0
        poly_assume = sym.Integer(0)
        for term in term_set:
            s = "s_" + str(mono_num)
            # constraint = "(assert (and (>= {} 0) (< {} {})))".format(s, s, 2**outbit)
            declare(s, "Int", output, silent)
            # print(constraint)
            mono_num += 1
            expr = "; " + s
            expr_sym = sym.Symbol(s)
            for t in term:
                expr += " * " + t
                expr_sym *= polys[t]
            if not silent:
                print(expr)
            if output:
                output.write(expr + "\n")
            expr_sym = expr_sym.expand()
            poly_assume += expr_sym

        poly_assume = poly_assume.as_expr()

        # Coefficient Calculation
        all_vars = []
        for var in vars.keys():
            bit = vars[var]
            for b in range(bit):
                all_vars.append(sym.Symbol(var + "_" + str(b)))

        for term in terms:
            t = term.split('*')

            if len(t) > 1:
                coeff = int(t.pop(0)) % (2**outbit)
                coeff = coeff if coeff >= 0 else coeff + 2**outbit
                expr = sym.Integer(1)
                for m in t:
                    expr *= sym.Symbol(m)
                expr = expr.as_expr()

                quo = poly_div(poly_assume, expr, all_vars)
                
                assert_coeff(quo, coeff, 2**outbit, output, silent)

                poly_assume = poly_assume - (quo * expr)
                poly_assume = poly_assume.expand()
            else:
                coeff = int(t.pop(0)) % (2**outbit)
                coeff = coeff if coeff >= 0 else coeff + 2**outbit
                const = sym.Integer(0)
                monomials = poly_assume.args
                for mono in monomials:
                    keep = True
                    for m in mono.args:
                        if m in all_vars:
                            keep = False
                            break
                    if keep:
                        const = const + mono

                assert_coeff(const, coeff, 2**outbit, output, silent)
                poly_assume = poly_assume - const
                poly_assume = poly_assume.expand()

        # Assert unused coefficient to be 0
        all_combs = []
        for term in term_set:
            term = list(term)
            if len(term) == 0:
                continue
            if len(term) == 1:
                var = term[0]
                for i in range(vars[var]):
                    all_combs.append(sym.Symbol(var + "_" + str(i)))
            else:
                partial_comb = []
                var1 = term[0]
                var2 = term[1]
                for i in range(vars[var1]):
                    var1_sym = sym.Symbol(var1 + "_" + str(i))
                    for j in range(vars[var2]):
                        var2_sym = sym.Symbol(var2 + "_" + str(j))
                        partial_comb.append(var1_sym * var2_sym)

                for i in range(2, len(term)):
                    partial_comb_updated = []
                    var = term[i]
                    for j in range(vars[var]):
                        var_sym = sym.Symbol(var + "_" + str(j))
                    for p in partial_comb:
                        updated = p * var_sym
                        partial_comb_updated.append(updated.expand())
                    partial_comb = partial_comb_updated

                all_combs += partial_comb

        for comb in all_combs:
            quo = poly_div(poly_assume, comb, all_vars)
            # print(comb, quo)
            if quo == 0:
                continue

            assert_coeff(quo, 0, 2**outbit, output, silent)
            poly_assume = poly_assume - (quo * comb)
            poly_assume = poly_assume.expand()

        if poly_assume != 0:
            assert_coeff(poly_assume, 0, 2**outbit, output, silent)

        # print(all_combs)

        if not silent:
            print("(check-sat)\n(get-model)")
        if output:
            output.write("(check-sat)\n(get-model)\n")

    print("done")

def main():
    args = parse_arguments()

    rewrite_smt(args=args, silent=True)

    

if __name__ == '__main__':
    main()