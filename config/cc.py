import checks

def apply_cc(env, cc):
    if cc in ['gcc']:
        pass
        #env.Tool(cc)
    elif cc == 'xlc':
        pass

    conf = env.Configure(
        custom_tests={
            'CheckCCFixed': checks.CheckCCFixed
            }
        )
    if not conf.CheckCCFixed():
        env.Exit()
    return conf.Finish()
