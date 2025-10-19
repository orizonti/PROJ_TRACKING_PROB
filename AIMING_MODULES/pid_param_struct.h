#ifndef PIDPARAMSTRUCT_H
#define PIDPARAMSTRUCT_H

class PIDParamStruct
{
public:
    PIDParamStruct() {RateParam = 0; DiffParam = 0; IntParam = 0;};
    PIDParamStruct(double Rate, double Diff, double Int) { RateParam = Rate; DiffParam = Diff; IntParam = Int;};
	double RateParam;
	double DiffParam;
	double IntParam;
	double Common;

	void operator=(PIDParamStruct Param)
	{
		this->Common = Param.Common;
		this->DiffParam = Param.DiffParam;
		this->IntParam = Param.IntParam;
		this->RateParam = Param.RateParam;
	}

    bool operator==(PIDParamStruct Param)
    {
        return (this->Common    == Param.Common    &&
                this->DiffParam == Param.DiffParam &&
                this->IntParam  == Param.IntParam  &&
                this->RateParam == Param.RateParam);

    }

    bool operator!=(PIDParamStruct Param)
    {
          return !(*this == Param);
    }
};
#endif // PIDPARAMSTRUCT_H
